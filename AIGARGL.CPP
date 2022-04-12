/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aigargl.cpp  -
   Author:   Lan Zhou
   
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
static LONG FlyHeight = 100;
static LONG FlyDownDistance = 400;
static LONG WingFlappingFrame = 2;
/* ========================================================================
   Function	- Gargoyles
   Description - AI function to bounce off the monster box looking for 
   			     the "player" to attack.
   Returns	 - Current state
   ======================================================================== */
void CAvatar::Gargoyles (CAvatar *pAvatar, CAvatar::AISTATUS Status)
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
			pAvatar->Status = AI_STONE;
			pAvatar->attrib.IssuedChallange = FALSE;
			pAvatar->hEnemy = fERROR;
			pAvatar->lastBump = iNOTHING;
			pAvatar->bumpTimer = 100;
			pAvatar->dAngle = 8;
			pAvatar->fContactDistance = 40;
	  		
			pAvatar->mfStartAnimationLoop(STANDSEQ);


			pAvatar->fGargoyle.Movement.dx = 0;
			pAvatar->fGargoyle.Movement.dy = pAvatar->mfGetMarchRate();
			pAvatar->fGargoyle.Movement.dz = 0;

			
			if (pAvatar->mfTeleportHome())
			{
				// Remember where "Home" is.
				pAvatar->fGargoyle.HomeX = pAvatar->mfX();
				pAvatar->fGargoyle.HomeY = pAvatar->mfY();
				pAvatar->fGargoyle.HomeZ = pAvatar->mfZ();
				pAvatar->fGargoyle.HomeA = pAvatar->mfAngle();
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
			// Dead is dead. Don't move. But make sure you are on the ground
			pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
			pAvatar->mfMoveToXYZA(pAvatar->mfX(),
								  pAvatar->mfY(),
								  FloorHeight,
								  pAvatar->mfAngle());

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
				pAvatar->Status = AI_STONE;
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
					pAvatar->Status = AI_STONE;
					//pAvatar->attrib.IssuedChallange = FALSE;
					pAvatar->mfStartAnimationOnce(STANDSEQ);
					pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
					pAvatar->mfMoveToXYZA(pAvatar->mfX(),
										  pAvatar->mfY(),
										  FloorHeight,
										  pAvatar->mfAngle());

			   		//pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
			   		//pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9); 

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
			   			//pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);

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
						//pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
						//pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9); 
							
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
					pAvatar->Status = AI_STONE;
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
						pAvatar->Status = AI_STONE;
						//pAvatar->attrib.IssuedChallange = FALSE;
						pAvatar->mfStartAnimationOnce(STANDSEQ);
						pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
						pAvatar->mfMoveToXYZA(pAvatar->mfX(),
											  pAvatar->mfY(),
											  FloorHeight,
											  pAvatar->mfAngle());

			   			//pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
  						//pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9); 

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
				   			//pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);

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
			   				//pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
							//pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9); 

							pAvatarTarget->mfDisEngage();
							// Maybe Add a goto here.
						}
					}
				}
			}
			else
			{
				// GWP You have no Player stats....Hit Points????
				pAvatar->Status = AI_STONE;
				//pAvatar->attrib.IssuedChallange = FALSE;
				if(pAvatar->attrib.IsFollowPath == TRUE)
				{
					//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
					pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
					pAvatar->DoAI(CAvatar::AI_INIT);
				}

			}
			break;
	
	case AI_STONE:		// Stay in this mode until activated.
			
			pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
			
			pAvatar->hEnemy = pAvatar->mfFindClosestFoe();
			
			if (pAvatar->hEnemy != fERROR)
			{
				CAvatar * pAvatarTarget = (CAvatar *) BLKPTR(pAvatar->hEnemy);
				const LONG distance = aprox_dist(pAvatar->mfX(), pAvatar->mfY(),
										 pAvatarTarget->mfX(), pAvatarTarget->mfY());
			
			
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
							height = pAvatarTarget->mfZ() + 10;		   //HACK!!
						}
						pAvatar->Status = AI_MOVING;
						pAvatar->bumpTimer = 100;
						pAvatar->mfStartAnimationOnce(MARCHSEQ);
						//pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
						//pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9); 

						pAvatar->FaceTo(pAvatarTarget->mfX(), pAvatarTarget->mfY());
						pAvatar->mfMoveToXYZA(pAvatar->mfX(),
											  pAvatar->mfY(),
											  height,
											  pAvatar->mfAngle());

						//AddSndObj(SND_HARPY_TAUNT1, SND_HARPY_TAUNT_TOTAL, pAvatar->ThingIndex );
					}
				}
			}
		
			if (pAvatar->Status == AI_STONE)
			{
				pAvatar->hEnemy = fERROR;
				//pAvatar->attrib.IssuedChallange = FALSE;
				// No foes seen yet.
				pAvatar->mfStartAnimationOnce(STANDSEQ);
				pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
				pAvatar->mfMoveToXYZA(pAvatar->mfX(),
									  pAvatar->mfY(),
									  FloorHeight,
									  pAvatar->mfAngle());


			}
			break;
	
	case AI_MOVING:		// Chase the player

			if (pAvatar->hEnemy == fERROR)
			{
				pAvatar->Status = AI_STONE;
				//pAvatar->attrib.IssuedChallange = FALSE;
				pAvatar->bumpTimer = 100;
				pAvatar->mfStartAnimationOnce(STANDSEQ);
				pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
				pAvatar->mfMoveToXYZA(pAvatar->mfX(),
									  pAvatar->mfY(),
									  FloorHeight,
									  pAvatar->mfAngle());

				//pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
				//pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9); 

				if(pAvatar->attrib.IsFollowPath == TRUE)
				{
					//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
					pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
					pAvatar->DoAI(CAvatar::AI_INIT);
					break;
				}

				break;
			}
			
			if (pAvatar->Engaged > 0)
			{
				// We're being attacked from the back. find'm and attack back.
				pAvatar->hEnemy = pAvatar->mfFindClosestFoe();
				//CAvatar * pAvatarTarget = (CAvatar *) BLKPTR(pAvatar->hEnemy);

				pAvatar->mfStartAnimationOnce(MARCHSEQ);
				//pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
				//pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9); 

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
					pAvatar->Status = AI_STONE;
					pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
					pAvatar->mfMoveToXYZA(pAvatar->mfX(),
										  pAvatar->mfY(),
										  FloorHeight,
										  pAvatar->mfAngle());

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
				pAvatar->Status = AI_STONE;
				//pAvatar->attrib.IssuedChallange = FALSE;
				if(pAvatar->attrib.IsFollowPath == TRUE)
				{
					//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
					pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
					pAvatar->DoAI(CAvatar::AI_INIT);
					break;
				}

				pAvatar->mfStartAnimationOnce(STANDSEQ);
				pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
				pAvatar->mfMoveToXYZA(pAvatar->mfX(),
									  pAvatar->mfY(),
									  FloorHeight,
									  pAvatar->mfAngle());

			   	//pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
			   	//pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9); 
					
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
				//pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
				//pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9); 

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
						Rotate((POINT*)&tp, pAvatar->mfAngle()+128);
						pAvatar->mfStartAnimationOnce(STANDSEQ);
						//pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
						//pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9); 

						pAvatar->Status = AI_STONE;
						if(pAvatar->attrib.IsFollowPath == TRUE)
						{
							//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
							pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
							pAvatar->DoAI(CAvatar::AI_INIT);
							break;
						}

						//pAvatar->attrib.IssuedChallange = FALSE;
						pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
						pAvatar->mfMoveToXYZA(pAvatar->mfX(),
											  pAvatar->mfY(),
											  FloorHeight,
											  pAvatar->mfAngle());


					}
					else
					{
						MoveAngle=AngleFromPoint2(pAvatar->mfX(), pAvatar->mfY(), pAvatarTarget->mfX(), pAvatarTarget->mfY(), 0);

						Rotate((POINT*)&tp, MoveAngle+pAvatar->dAngle);

						pAvatar->mfStartAnimationOnce(MARCHSEQ);
						//pAvatar->mfQueueSnd(SND_HARPY_WINGFLAP1, SND_HARPY_WINGFLAP_TOTAL, WingFlappingFrame);
						//pAvatar->mfSetLoopingSnd(SND_HARPY_WINGFLAP1, 9); 
						
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

