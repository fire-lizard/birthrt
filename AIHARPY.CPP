/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aiharpy.cpp  -
   Author:   Lan Zhou

   ========================================================================

   Contains the following general functions:


   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "avatar.hxx"
#include "playstat.hxx"
#include "sound.hxx"

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
static LONG FlyHeight = 100;
static LONG FlyDownDistance = 400;
static LONG WingFlappingFrame = 2;
/* ========================================================================
   Function	- Harpies
   Description - AI function to bounce off the monster box looking for
   			     the "player" to attack.
   Returns	 - Current state
   ======================================================================== */
void CAvatar::Harpies (CAvatar *pAvatar, CAvatar::AISTATUS Status)
{
	LONG FloorHeight;
	LONG CeilingHeight;
	LONG Special;
	
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
			pAvatar->attrib.IssuedChallange = FALSE;
			pAvatar->attrib.IsFlying = TRUE;
			pAvatar->hEnemy = fERROR;
			pAvatar->lastBump = iNOTHING;
			pAvatar->bumpTimer = 100;
			pAvatar->dAngle = 8;
			pAvatar->fContactDistance = 40;
			
			if (pAvatar->mfStandWaiting())
			{
				pAvatar->mfStartAnimationLoop(STANDSEQ);
			}
			else
			{
				pAvatar->mfStartAnimationLoop(WALKSEQ);
				pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
				pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9);

				pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
				pAvatar->mfMoveToXYZA(pAvatar->mfX(),
									  pAvatar->mfY(),
				   					  (FloorHeight+FlyHeight),
				   					  pAvatar->mfAngle());

			}

			pAvatar->fHarpy.Movement.dx = 0;
			pAvatar->fHarpy.Movement.dy = pAvatar->mfGetWalkRate();
			pAvatar->fHarpy.Movement.dz = 0;

			
			if (pAvatar->mfTeleportHome())
			{
				// Remember where "Home" is.
				pAvatar->fHarpy.HomeX = pAvatar->mfX();
				pAvatar->fHarpy.HomeY = pAvatar->mfY();
				pAvatar->fHarpy.HomeZ = pAvatar->mfZ();
				pAvatar->fHarpy.HomeA = pAvatar->mfAngle();
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
				pAvatar->attrib.IsFlying = FALSE;
				pAvatar->mfClearLoopingSnd(SND_HARPY_WINGFLAP1);
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

			if(pAvatar->mfTestSequenceDone() &&
			     pAvatar->CurSequence != DEADSEQ)
			{
		   		AddSndObj(SND_BODY_FALLS_DEAD1, SND_BODY_FALLS_DEAD_TOTAL, pAvatar->ThingIndex );
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
				//pAvatar->attrib.IssuedChallange = FALSE;
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
					//pAvatar->attrib.IssuedChallange = FALSE;
					pAvatar->mfStartAnimationOnce(WALKSEQ);
			   		pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
			   		pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9);

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
		
					if (pAvatar->mfCanHarpyHitHim(pAvatarTarget, distance, pAvatar->fContactDistance)
						||((pAvatarTarget->mfHeight()<60)&&pAvatar->mfCanIHitHim(pAvatarTarget, distance, pAvatar->fContactDistance)))
					{
						pAvatar->FaceTo(pAvatarTarget->mfX(),
						                pAvatarTarget->mfY());

							// [d12-12-96 JPC] Not needed?  When I commented the
							// following WALKSEQ line out, the harpy failed to come
							// after me right away.  So I left it in.
							pAvatar->mfStartAnimationOnce(WALKSEQ);

							pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);

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
						pAvatar->mfStartAnimationOnce(MARCHSEQ);
						pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
						pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9);
							
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
					//pAvatar->attrib.IssuedChallange = FALSE;
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
						//pAvatar->attrib.IssuedChallange = FALSE;
						pAvatar->mfStartAnimationOnce(WALKSEQ);
			   			pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
  						pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9);

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
						
						if (pAvatar->mfCanHarpyHitHim(pAvatarTarget, distance, pAvatar->fContactDistance)
							||((pAvatarTarget->mfHeight()<60)&&pAvatar->mfCanIHitHim(pAvatarTarget, distance, pAvatar->fContactDistance)))
						{
							pAvatar->FaceTo(pAvatarTarget->mfX(),
							                pAvatarTarget->mfY());
							// [d12-11-96 JPC] Don't start a sequence here.
							// Doing so prevents the harpy from hitting us.
							// pAvatar->mfStartAnimationOnce(WALKSEQ);
				   			pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);

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
							pAvatar->mfStartAnimationOnce(MARCHSEQ);
			   				pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
							pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9);

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
				//pAvatar->attrib.IssuedChallange = FALSE;
				if(pAvatar->attrib.IsFollowPath == TRUE)
				{
					//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
					pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
					pAvatar->DoAI(CAvatar::AI_INIT);
				}

			}
			break;
	
	case AI_SEARCH:		// Stay in this mode until activated.
			
			pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
			
			pAvatar->hEnemy = pAvatar->mfFindClosestFoe();
			
			if (pAvatar->hEnemy != fERROR)
			{
				CAvatar * pAvatarTarget = (CAvatar *) BLKPTR(pAvatar->hEnemy);
				const LONG distance = aprox_dist(pAvatar->mfX(), pAvatar->mfY(),
										 pAvatarTarget->mfX(), pAvatarTarget->mfY());
			
				// If you can be seen you are active no matter what.
				//if (pAvatar->mfIsVisible())
				//{
				//	pAvatar->Status = AI_MOVING;
				//	pAvatar->mfMoveToXYZA(pAvatar->mfX(),
				//						  pAvatar->mfY(),
				//						  (FloorHeight+FlyHeight),
				//						  pAvatar->mfAngle());
				//	
				//}
				//else
				if (pAvatar->mfCanSeeAvatar(pAvatarTarget) &&
					pAvatarTarget->Engaged <= 3)
				{
  							LONG height;
					
					if (distance <= CAvatar::ActivationDistance)
					{	
						if(FloorHeight < pAvatarTarget->mfZ()
							&& distance > 150)
						{
							if((pAvatarTarget->mfZ() + FlyHeight + pAvatar->mfHeight()) > CeilingHeight)
								height = CeilingHeight - pAvatar->mfHeight();
							else
								height = pAvatarTarget->mfZ() + FlyHeight;
						}
						else if(distance > 150)
						{
							height = FloorHeight + FlyHeight;
						}
						else
						{
							height = pAvatarTarget->mfZ();		   //HACK!!
						}
						pAvatar->Status = AI_MOVING;
						pAvatar->bumpTimer = 100;
						pAvatar->mfStartAnimationOnce(MARCHSEQ);
						pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
						pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9);

						pAvatar->FaceTo(pAvatarTarget->mfX(), pAvatarTarget->mfY());
						pAvatar->mfMoveToXYZA(pAvatar->mfX(),
											  pAvatar->mfY(),
											  height,
											  pAvatar->mfAngle());

						if (pAvatar->attrib.IssuedChallange == FALSE)
						{
							AddSndObj(SND_HARPY_TAUNT1, SND_HARPY_TAUNT_TOTAL, pAvatar->ThingIndex );
							pAvatar->attrib.IssuedChallange = TRUE;
						}
					}
				}
			}
		
			if (pAvatar->Status == AI_SEARCH)
			{
				pAvatar->hEnemy = fERROR;
				//pAvatar->attrib.IssuedChallange = FALSE;
				// No foes seen yet.
				if (pAvatar->mfStandWaiting())
				{
					pAvatar->mfStartAnimationOnce(STANDSEQ);
				}
				else
				{
					// Wander around inside MonsterBox.
					LONG Angle;
					LONG MythingBumped;
					const LONG tx = pAvatar->mfX() + pAvatar->fHarpy.Movement.dx;
					const LONG ty = pAvatar->mfY() + pAvatar->fHarpy.Movement.dy;
					const LONG distance = aprox_dist(
											   pAvatar->mfX(),
					                           pAvatar->mfY(),
					                           tx,
					                           ty);
					LONG tz;
					if(pAvatar->mfZ() < (FloorHeight + FlyHeight))
						tz = FloorHeight + FlyHeight;
					else
						tz = 0;
					// GWP Do we really need to set our position twice?
					//pAvatar->mfStartAnimationOnce(WALKSEQ);
					
					//pAvatar->mfMoveToXYZA(pAvatar->mfX(),
					//					  pAvatar->mfY(),
					//					  (FloorHeight+FlyHeight),
					//					  pAvatar->mfAngle());
					
					pAvatar->mfStartAnimationOnce(WALKSEQ);
					pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
					pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9);

					const WadThingType bump=pAvatar->mfFlyTowardWithBump(
					                            distance,
												tx,
							                    ty,
												tz,
							                    0,
							                    pAvatar->mfGetWalkRate(),
							                    CHECKLINE_MONSTER,
							                    &Angle,
												&MythingBumped);
					if(bump != iNOTHING
					   &&bump != iFLOOR
					   &&bump != iOBJECT
					   &&bump != iCEILING
					   &&bump != iSLIDE_ON_WALL)
					{
						// We didn't move.
						POINT	tp;
						LONG	NewAngle;
						
						//GWP move Rate amount in a reflective direction
						tp.x = 0;
						tp.y = pAvatar->mfGetWalkRate();
						pAvatar->mfReflect(Angle, NewAngle);

						if(pAvatar->lastBump == iWALL
							&& pAvatar->bumpTimer<15)
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
						pAvatar->fHarpy.Movement.dx = tp.x;
						pAvatar->fHarpy.Movement.dy = tp.y;
					}
				}
			}
			break;
	
	case AI_MOVING:		// Chase the player

			if (pAvatar->hEnemy == fERROR)
			{
				pAvatar->Status = AI_SEARCH;
				//pAvatar->attrib.IssuedChallange = FALSE;
				pAvatar->bumpTimer = 100;
				pAvatar->mfStartAnimationOnce(WALKSEQ);
				pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
				pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9);

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
				//CAvatar * pAvatarTarget = (CAvatar *) BLKPTR(pAvatar->hEnemy);

				pAvatar->mfStartAnimationOnce(MARCHSEQ);
				pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
				pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9);

				//const LONG distance = aprox_dist(pAvatar->mfX(), pAvatar->mfY(),
				//							 pAvatarTarget->mfX(), pAvatarTarget->mfY());
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
			
			if (distance > CAvatar::ActivationDistance && !pAvatar->mfCanSeeAvatar(pAvatarTarget)) // Got away
			{
				pAvatar->Status = AI_SEARCH;
				//pAvatar->attrib.IssuedChallange = FALSE;
				if(pAvatar->attrib.IsFollowPath == TRUE)
				{
					//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
					pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
					pAvatar->DoAI(CAvatar::AI_INIT);
					break;
				}

				if (pAvatar->mfTeleportHome())
				{
					pAvatar->mfStartAnimationOnce(WALKSEQ);
					pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
					pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9);
					
					pAvatar->mfMoveToXYZA(pAvatar->fHarpy.HomeX,
										  pAvatar->fHarpy.HomeY,
										  pAvatar->fHarpy.HomeZ,
										  pAvatar->fHarpy.HomeA);
				}
			}
			else
			if (pAvatar->mfCanHarpyHitHim(pAvatarTarget, distance, pAvatar->fContactDistance)
				||((pAvatarTarget->mfHeight()<60)&&pAvatar->mfCanIHitHim(pAvatarTarget, distance, pAvatar->fContactDistance)))
				
			{
				pAvatar->Status = AI_ATTACK;
				pAvatarTarget->mfEngage();

			}
			else
			{
				LONG Angle;
				FIXED_VECTOR_3D tp;
				LONG MoveAngle;
				LONG MythingBumped;
				// Chase the Target.
				
				// GWP If we hit something do we care?
				// GWP maybe we should try a slightly offset angle from
				// GWP the first try and try again.
				pAvatar->mfStartAnimationOnce(MARCHSEQ);
				pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
				pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9);

				const WadThingType bump = pAvatar->mfFlyTowardWithBump(
				                            distance,
											pAvatarTarget->mfX(),
						                    pAvatarTarget->mfY(),
											pAvatarTarget->mfZ() + ((distance > FlyDownDistance) ? FlyHeight : 0),
						                    FlyDownDistance,
											pAvatar->mfGetMarchRate(),
						                    (SHORT)((TRUE == pAvatar->mfStayInMonsterBox())
									                 ? CHECKLINE_MONSTER : 0),
						                    &Angle,
											&MythingBumped);		
				switch(bump)
				{			
					
				case iNOTHING:
				case iSLIDE_ON_WALL:
				case iFLOOR:
				case iCEILING:
					pAvatar->dAngle = 8;
					break;
				default:
			   		// we're stuck.
										// we're stuck.
					tp.dx = 0;
					tp.dy = (pAvatar->mfGetMarchRate())<<PLAYER_FIXEDPT;
					tp.dz = 0;
					if(pAvatar->dAngle > 64)
					{	
						//Rotate((POINT*)&tp, pAvatar->mfAngle());
						pAvatar->mfStartAnimationOnce(WALKSEQ);
						pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
						pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9);

						pAvatar->Status = AI_SEARCH;
						if(pAvatar->attrib.IsFollowPath == TRUE)
						{
							//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
							pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
							pAvatar->DoAI(CAvatar::AI_INIT);
							break;
						}
						pAvatar->dAngle=8;

						pAvatar->attrib.IssuedChallange = FALSE;
						//pAvatar->mfMoveToXYZA((pAvatar->mfX()+PLAYER_INT_VAL(tp.dx)),
						//					  (pAvatar->mfY()+PLAYER_INT_VAL(tp.dy)),
						//					  pAvatar->mfZ() + tp.dz,		//Hack
						//					   (pAvatar->mfAngle()+128));

					}
					else
					{
						MoveAngle=AngleFromPoint2(pAvatar->mfX(), pAvatar->mfY(), pAvatarTarget->mfX(), pAvatarTarget->mfY(), 0);

						Rotate((POINT*)&tp, MoveAngle+pAvatar->dAngle);

						pAvatar->mfStartAnimationOnce(MARCHSEQ);
						pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
						pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9);
						
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
					break;

				}
			}
			break;
			
	}
}

/* ========================================================================
   Function	- FlyTowardWithBump
   Description - Move to withing BUMP distance from a point by my Rate amount
   Returns	 - distance from point
   ======================================================================== */

WadThingType CAvatar::mfFlyTowardWithBump(
		LONG	distance,
		LONG	TargetX,
		LONG	TargetY,
		LONG    TargetZ,
		LONG	Bump,
		LONG	Rate,
		SHORT	sSpecial,
		LONG	*pAngle,
		LONG	*pmyThingBumped
)
{
	PLAYER TempPlayer;
	WadThingType Result = iNOTHING;
	FIXED_VECTOR_3D NewVector;
	LONG BumpDistance;
	LONG dz = TargetZ - mfZ();
	LONG dx = TargetX - mfX();
	LONG dy = TargetY - mfY();

	if(abs(distance) > Bump)
	{
			
		if (distance > 0)
			distance -= Bump;
		else
			distance += Bump;
		
		
	   //	if (Bump > 2)
	   //	{
			// Introduce some randomness as to exactly where
			// we endup within "Bump" distance of the target.
			// Determine whether we are to the N/S of the target or E/W
			// So we woble on the short axis.
	   //		if (ABS(dx) > ABS(dy))
	   //		{
	   //			if (random(2))
	   //			{
	   //				dy += random(Bump - 1);
	   //			}
	   //			else
	   //			{
	   //				dy -= random(Bump - 1);
	   //			}
	   //		}
	   //		else
	   //		{
	   //			if (random(2))
	   //			{
	   //				dx += random(Bump - 1);
	   //			}
	   //			else
	   //			{
	   //				dx -= random(Bump - 1);
	   //			}
	   //		}
	   //	}
		
		FaceTo(TargetX, TargetY);
		mfConvertPositionToPlayer(TempPlayer);
		
		// Fly up only if necessary.
		if (dz > 0)
		{
			if (dz < Rate)
			{
				NewVector.dz = dz;
		    }
			else
			{
				NewVector.dz = Rate;
			}
		}
		else
		{
			NewVector.dz = 0;
		}
		
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
		
		
		// For now ignore the noise we make if we hit something.
		// GWP optimize by not doing this check if we're not "visible."
		// GWP if we can't be seen bumping objects who cares?
		//if (mfIsVisible())
		{
			CheckBump(&TempPlayer, &NewVector, Rate);
		}
		
		// [d11-27-96 JPC] For flying things, result of deep water, lava,
		// or acid is the same as no bump, so change result.
		Result = CheckMove(&TempPlayer, &NewVector, sSpecial, pAngle, &BumpDistance);
		if (Result == iDEEP_WATER || 
		    Result == iSHALLOW_WATER || 
		    Result == iLAVA || 
		    Result == iACID)
		{
			Result = iNOTHING;
			TempPlayer.bump = iNOTHING;
		}

		switch (Result)
		{
		case iCEILING:
		case iFLOOR:
			mfMoveToXYZA(mfX(),
						 mfY(),
		  				 mfZ() + NewVector.dz,
						 mfAngle());
			break;
		case iWALL:
			break;
		case iNOTHING:
		case iSLIDE_ON_WALL:
		case iOBJECT:
			mfMoveToXYZA(mfX() + PLAYER_INT_VAL(NewVector.dx),
						 mfY() + PLAYER_INT_VAL(NewVector.dy),
	  					 mfZ() + NewVector.dz,
						 mfAngle());
			break;
		}

	}
	else
	{
	
		FaceTo(TargetX, TargetY);
		
		mfConvertPositionToPlayer(TempPlayer);
		if (ABS(distance) < Rate)
		{
			NewVector.dx = dx;
			NewVector.dy = dy;
			if (ABS(dz) < Rate)
			{
				NewVector.dz = dz;
		    }
			else
			{
				if(dz > 0)
					NewVector.dz = Rate;
				else
					NewVector.dz = -Rate;
			}
		}
		else
		{
			NewVector.dx = ((dx * Rate)/distance);
			NewVector.dy = ((dy * Rate)/distance);
			NewVector.dz = dz*Rate/ABS(distance);
		}
			
		NewVector.dx<<=PLAYER_FIXEDPT;
		NewVector.dy<<=PLAYER_FIXEDPT;
		

		if (mfIsVisible())
		{
			CheckBump(&TempPlayer, &NewVector, Rate);
		}

		Result = CheckMove(&TempPlayer, &NewVector, sSpecial, pAngle, &BumpDistance);
		if (Result == iDEEP_WATER || 
		    Result == iSHALLOW_WATER || 
		    Result == iLAVA || 
		    Result == iACID)
		{
			Result = iNOTHING;
			TempPlayer.bump = iNOTHING;
		}

		switch (Result)
		{
			case iSLIDE_ON_WALL:
			case iOBJECT:
			case iNOTHING:
				mfMoveToXYZA(mfX() + PLAYER_INT_VAL(NewVector.dx),
							 mfY() + PLAYER_INT_VAL(NewVector.dy),
							 mfZ() + NewVector.dz,
							 mfAngle());
				break;
			case iWALL:
				break;
			case iCEILING:
			case iFLOOR:
				mfMoveToXYZA(mfX(),
							 mfY(),
  							 mfZ() + NewVector.dz,
							 mfAngle());
			    break;
		}

	}
	*pmyThingBumped = TempPlayer.BumpIndex;
	return (TempPlayer.bump);
}
/* ========================================================================
   Function    - mfCanIHitHim
   Description - inline Check whether the Avatar hit the enmey.
   Returns     - TRUE | FALSE
   ======================================================================== */
BOOL CAvatar::mfCanHarpyHitHim (
	CAvatar *pAvatarEnemy,
	LONG distance,
	LONG AttackDistance) const
{
	BOOL Result;
	
	const LONG ArmHeight= mfZ() + ((3*mfHeight())/4);
	const LONG EnemyArmHeight= pAvatarEnemy->mfZ() + ((3*pAvatarEnemy->mfHeight())/4);
	const LONG HeadHeight = mfZ() + mfHeight();
	const LONG EnemyHeadHeight = pAvatarEnemy->mfZ() + pAvatarEnemy->mfHeight();

	// your arms must be above his knees and
	// his arms must be above your knees to attack.
	if (distance < AttackDistance &&
	    HeadHeight > EnemyArmHeight &&
	    EnemyHeadHeight > ArmHeight
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
