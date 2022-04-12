/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aiwyvern.cpp  -
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
#define HEIGHT 90   
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
static LONG FlyHeight1 = 150;
static LONG FlyHeight2 = 250;
static LONG WDistance1 = 200;
static LONG WDistance2 = 250;
static LONG FlyDownDistance = 1000;
static LONG WingFlappingFrame = 4;
/* ========================================================================
   Function	- Wyverns
   Description - AI function to bounce off the monster box looking for 
   			     the "player" to attack.
   Returns	 - Current state
   ======================================================================== */
void CAvatar::Wyverns (CAvatar *pAvatar, CAvatar::AISTATUS Status)
{
	LONG FloorHeight;
	LONG CeilingHeight;
	LONG Special;
	LONG flyHeight;
	LONG WalkDistance;
	LONG attack1Pts;
	LONG attack2Pts;
	BIRTHRT_SND waveId;
	SHORT waveRange;
	

	if( player.ThingIndex != fERROR &&
		!reject( pAvatar->mfGetSector(), mythings[player.ThingIndex].sect))
	{
		return;
	}

	if( GAME_TTYPE::mfIsLargeAvatar(pAvatar->mfType()))			   // big wyvern
	{
		flyHeight=FlyHeight2;
		WalkDistance = WDistance2;
		attack1Pts = 35;
		attack2Pts = 10;
		waveId = SND_WYVERN_WINGFLAP1;
		waveRange = SND_WYVERN_WINGFLAP_TOTAL;
	}
	else
	{
		flyHeight=FlyHeight1;
		WalkDistance = WDistance1;					// small wyvern
		attack1Pts = 10;
		attack2Pts = 5;
		waveId = SND_SMALL_WYVERN_WINGFLAP1;
		waveRange = SND_SMALL_WYVERN_WINGFLAP_TOTAL;
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
			pAvatar->hEnemy = fERROR;
			pAvatar->lastBump = iNOTHING;
			pAvatar->bumpTimer = 100;
			pAvatar->dAngle = 8;
			if (GAME_TTYPE::mfIsLargeAvatar(pAvatar->mfType()))
			{
				pAvatar->fContactDistance = 80;
			}
			else
			{
				pAvatar->fContactDistance = 40;
			}
			
			if (pAvatar->mfStandWaiting())
			{
				pAvatar->mfStartAnimationLoop(STANDSEQ);
			}
			else
			{
				pAvatar->mfStartAnimationLoop(MARCHSEQ);
				pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
				pAvatar->mfSetLoopingSnd(waveId, 9); 
				pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
				if(ABS(CeilingHeight - FloorHeight) >= 50)
				{
					pAvatar->attrib.IsFlying = TRUE;
					pAvatar->mfMoveToXYZA(pAvatar->mfX(),
										  pAvatar->mfY(),
				   						  (FloorHeight+flyHeight),
				   						  pAvatar->mfAngle());

				}
				else
				{
					pAvatar->attrib.IsFlying = FALSE;
				}
			}

			pAvatar->fWyvern.Movement.dx = 0;
			pAvatar->fWyvern.Movement.dy = pAvatar->mfGetMarchRate();
			pAvatar->fWyvern.Movement.dz = 0;

			
			if (pAvatar->mfTeleportHome())
			{
				// Remember where "Home" is.
				pAvatar->fWyvern.HomeX = pAvatar->mfX();
				pAvatar->fWyvern.HomeY = pAvatar->mfY();
				pAvatar->fWyvern.HomeZ = pAvatar->mfZ();
				pAvatar->fWyvern.HomeA = pAvatar->mfAngle();
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
			pAvatar->mfClearLoopingSnd(waveId);
			//if (pAvatar->mfGetArtSequence() != DEADSEQ)
			//{
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
Defend_Yourself:
			if (pAvatar->hEnemy == fERROR)
			{
				pAvatar->Status = AI_SEARCH;
				pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
				pAvatar->mfSetLoopingSnd(waveId, 9); 

				//pAvatar->attrib.IssuedChallange = FALSE;
				if(pAvatar->attrib.IsFollowPath == TRUE)
				{
					pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
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
					pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
					pAvatar->mfSetLoopingSnd(waveId, 9); 


					//pAvatar->attrib.IssuedChallange = FALSE;
					pAvatar->mfStartAnimationOnce(MARCHSEQ);

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
		
					
					if(	pAvatar->mfCanIHitHim(pAvatarTarget, distance, pAvatar->fContactDistance))
					{
						pAvatar->FaceTo(pAvatarTarget->mfX(),
						                pAvatarTarget->mfY());
						pAvatar->mfClearLoopingSnd(waveId);
						if ((pAvatar->mfGetArtSequence() != ATTACK1SEQ &&
						     pAvatar->mfGetArtSequence() != ATTACK2SEQ &&
						     pAvatar->mfGetArtSequence() != DEFENDSEQ) ||
						     pAvatar->mfTestSequenceDone())
						{
							pAvatar->mfFightSounds(pAvatar->hEnemy);
							pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
							pAvatar->attrib.IsFlying = FALSE;
							pAvatar->mfMoveToXYZA(pAvatar->mfX(),
												  pAvatar->mfY(),
												  FloorHeight,
												  pAvatar->mfAngle());
							switch(random(5))
							{
							case 0:
							case 1:				// use attack2seq as defending sequence
												// use defendseq as attacking2 sequence(fight sequence)
							case 2:
							case 3:
								pAvatar->mfStartAnimationOnce(ATTACK2SEQ);
								break;
							case 4:
								pAvatar->mfStartAnimationOnce(DEFENDSEQ);
								break;
							}
						}
					}
					else
					{
						pAvatar->Status = AI_MOVING;
						pAvatar->mfStartAnimationOnce(MARCHSEQ);
						pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
						pAvatar->mfSetLoopingSnd(waveId, 9); 
							
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
					pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
					pAvatar->mfSetLoopingSnd(waveId, 9); 


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
				if (pPlayerStats->mfGetCurHitPoints() < 5) // GWP HACK Number!
				{
					pAvatar->Status = AI_DEFEND;	// Increase your chance of survival
					pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
					pAvatar->attrib.IsFlying = FALSE;
					pAvatar->mfMoveToXYZA(pAvatar->mfX(),
										  pAvatar->mfY(),
										  FloorHeight,
										  pAvatar->mfAngle());

					// GWP HACK! HACK!
					goto Defend_Yourself;
				}
				else
				{
					LONG attackHeight;
					CAvatar * pAvatarTarget = (CAvatar *) BLKPTR(pAvatar->hEnemy);
					
					if(pAvatarTarget->mfHeight() < pAvatar->mfHeight())
						 attackHeight = 0;
					else
						attackHeight = pAvatarTarget->mfHeight() - pAvatar->mfHeight();
					if (pAvatarTarget->mfAmIImmoblized())
					{
						pAvatar->Status = AI_SEARCH;
						pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
						pAvatar->mfSetLoopingSnd(waveId, 9); 


						//pAvatar->attrib.IssuedChallange = FALSE;
						pAvatar->mfStartAnimationOnce(MARCHSEQ);

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
						
						if(	pAvatar->mfCanIHitHim(pAvatarTarget, distance, pAvatar->fContactDistance))
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
								// When the wyvern is attacking, let everyone hack away
								// at each other.
								pAvatar->mfClearTurnToAttack();
								if (pAvatarTarget->hEnemy == pAvatar->hThis)
								{
									pAvatarTarget->mfClearTurnToAttack();
								}
								
								pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
								if (pPlayerStats->mfGetCurHitPoints() > attack1Pts) // GWP HACK Number!
								{
									pAvatar->mfClearLoopingSnd(waveId);

									pAvatar->mfStartAnimationOnce(ATTACK1SEQ);
									pAvatar->attrib.IsFlying = TRUE;
									pAvatar->mfMoveToXYZA(pAvatar->mfX(),
														  pAvatar->mfY(),
														  FloorHeight + attackHeight,
														  pAvatar->mfAngle());
								}
								else 
								{	
									pAvatar->mfClearLoopingSnd(waveId);
									pAvatar->attrib.IsFlying = FALSE;
									pAvatar->mfMoveToXYZA(pAvatar->mfX(),
														  pAvatar->mfY(),
														  FloorHeight,
														  pAvatar->mfAngle());
									
									if(random(2))
									{
										pAvatar->mfStartAnimationOnce(DEFENDSEQ);
									}
									else
									{
										pAvatar->mfStartAnimationOnce(ATTACK2SEQ);
									}
								}
							}
						}
						else
						{
							pAvatar->Status = AI_MOVING;
							if(pAvatar->mfZ() < (FloorHeight+40) && distance < WalkDistance)
							{
								pAvatar->attrib.IsFlying = FALSE;
								pAvatar->mfMoveToXYZA(pAvatar->mfX(), pAvatar->mfY(),
													  FloorHeight,
													  pAvatar->mfAngle());
								pAvatar->mfStartAnimationOnce(WALKSEQ);
							}
							else
							{
								pAvatar->mfStartAnimationOnce(MARCHSEQ);
								pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
								pAvatar->mfSetLoopingSnd(waveId, 9); 

							}
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
				pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
				pAvatar->mfSetLoopingSnd(waveId, 9); 

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
			
				
				if (pAvatar->mfCanSeeAvatar(pAvatarTarget) && 
					pAvatarTarget->Engaged <= 3)
				{
  							LONG height;
					
					if (distance <= CAvatar::ActivationDistance)
					{	
						if(FloorHeight < pAvatarTarget->mfZ()
							&& distance > WalkDistance)
						{
							if((pAvatarTarget->mfZ() + flyHeight + pAvatar->mfHeight()) > CeilingHeight)
								height = CeilingHeight - pAvatar->mfHeight();
							else
								height = pAvatarTarget->mfZ() + flyHeight;
							pAvatar->mfStartAnimationOnce(MARCHSEQ);
							pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
							pAvatar->mfSetLoopingSnd(waveId, 9); 

						}
						else if(distance > WalkDistance && FloorHeight <= 50)
						{
							pAvatar->mfStartAnimationOnce(MARCHSEQ);
							pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
							pAvatar->mfSetLoopingSnd(waveId, 9); 

							height = FloorHeight + flyHeight;
						} 
						else if(distance <=WalkDistance && pAvatar->mfZ() < (FloorHeight+40) && pAvatarTarget->mfZ() == FloorHeight)
						{
							pAvatar->mfStartAnimationOnce(WALKSEQ);
							height = FloorHeight;
						}
						else
						{
							pAvatar->mfStartAnimationOnce(MARCHSEQ);
							pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
							pAvatar->mfSetLoopingSnd(waveId, 9); 

							height = pAvatarTarget->mfZ() + 10;		   //HACK!!
						}
						pAvatar->Status = AI_MOVING;
						pAvatar->bumpTimer = 100;
						pAvatar->FaceTo(pAvatarTarget->mfX(), pAvatarTarget->mfY());
						
						if (height > FloorHeight)
							pAvatar->attrib.IsFlying = TRUE;
						else
							pAvatar->attrib.IsFlying = FALSE;
						pAvatar->mfMoveToXYZA(pAvatar->mfX(),
											  pAvatar->mfY(),
											  height,
											  pAvatar->mfAngle());

						AddSndObj(SND_WYVERN_SCREAM1, SND_WYVERN_SCREAM_TOTAL, pAvatar->ThingIndex );
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
					const LONG tx = pAvatar->mfX() + pAvatar->fWyvern.Movement.dx;
					const LONG ty = pAvatar->mfY() + pAvatar->fWyvern.Movement.dy;
					const LONG distance = aprox_dist(
											   pAvatar->mfX(),
					                           pAvatar->mfY(),
					                           tx,
					                           ty);
					LONG tz;
					if(FloorHeight <= 50 && pAvatar->mfZ() < (FloorHeight + flyHeight))
						tz = FloorHeight + flyHeight;
					else 
						tz = 0;
					// GWP Do we really need to set our position twice?
					//pAvatar->mfStartAnimationOnce(MARCHSEQ);
					
					//pAvatar->mfMoveToXYZA(pAvatar->mfX(),
					//					  pAvatar->mfY(),
					//					  (FloorHeight+flyHeight),
					//					  pAvatar->mfAngle());
					
					pAvatar->mfStartAnimationOnce(MARCHSEQ);
					pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
					pAvatar->mfSetLoopingSnd(waveId, 9); 

					const WadThingType bump=pAvatar->mfFlyTowardWithBump(
					                            distance,
												tx,
							                    ty,
												tz,
							                    0,
							                    pAvatar->mfGetMarchRate(),
							                    CHECKLINE_MONSTER,
							                    &Angle,
												&MythingBumped);
					if (!(bump == iNOTHING ||
					      bump == iSHALLOW_WATER ||
					      bump == iDEEP_WATER ||
					      bump == iLAVA ||
					      bump == iACID ||
					      bump == iHOLE )
					   )
					{
						// We didn't move.
						POINT	tp;
						LONG	NewAngle;
						
						//GWP move Rate amount in a reflective direction
						tp.x = 0;
						tp.y = pAvatar->mfGetMarchRate();
						pAvatar->mfReflect(Angle, NewAngle);

						if((pAvatar->lastBump == iWALL ||
						    pAvatar->lastBump == iMONSTER_BOX)
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
						pAvatar->fWyvern.Movement.dx = tp.x;
						pAvatar->fWyvern.Movement.dy = tp.y;
					}
				}
			}
			break;
	
	case AI_MOVING:		// Chase the player
			
			pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);

			if (pAvatar->hEnemy == fERROR)
			{
				pAvatar->Status = AI_SEARCH;
				//pAvatar->attrib.IssuedChallange = FALSE;
				pAvatar->bumpTimer = 100;
				pAvatar->mfStartAnimationOnce(MARCHSEQ);
				pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
				pAvatar->mfSetLoopingSnd(waveId, 9); 

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
				if(pAvatar->mfZ() == FloorHeight)
					pAvatar->mfStartAnimationOnce(WALKSEQ);
				else
					pAvatar->mfStartAnimationOnce(MARCHSEQ);
					pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
					pAvatar->mfSetLoopingSnd(waveId, 9); 

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
					pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
					pAvatar->mfSetLoopingSnd(waveId, 9); 

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
				pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
				pAvatar->mfSetLoopingSnd(waveId, 9); 

				if(pAvatar->attrib.IsFollowPath == TRUE)
				{
					//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
					pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
					pAvatar->DoAI(CAvatar::AI_INIT);
					break;
				}

				if (pAvatar->mfTeleportHome())
				{
					pAvatar->mfStartAnimationOnce(MARCHSEQ);
					pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
					pAvatar->mfSetLoopingSnd(waveId, 9); 
					
					pAvatar->mfMoveToXYZA(pAvatar->fWyvern.HomeX,
										  pAvatar->fWyvern.HomeY,
										  pAvatar->fWyvern.HomeZ,
										  pAvatar->fWyvern.HomeA);
				}
			}
			else
			if(	pAvatar->mfCanIHitHim(pAvatarTarget, distance, pAvatar->fContactDistance))
			{
				pAvatar->Status = AI_ATTACK;
				pAvatarTarget->mfEngage();

			}
			else
			if(pAvatar->mfZ() == FloorHeight && pAvatarTarget->mfZ() == FloorHeight
			   && distance < WalkDistance)
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
				pAvatar->mfStartAnimationOnce(WALKSEQ);
				pAvatar->attrib.IsFlying = FALSE;
				const WadThingType bump = pAvatar->mfMoveTowardWithBump(
				                            distance,
											pAvatarTarget->mfX(),
						                    pAvatarTarget->mfY(),
						                    pAvatar->fContactDistance/3,	// GWP HACK VALUE
						                    pAvatar->mfGetWalkRate(),
						                    (SHORT)((TRUE == pAvatar->mfStayInMonsterBox()) 
									                 ? CHECKLINE_MONSTER : 0),
						                    &Angle,
						                    &myThingBumped);
				if ( bump == iSLIDE_ON_WALL ||
					bump == iFLOOR||
					bump == iCEILING ||
					bump == iWALL ||
					bump == iMONSTER_BOX)
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

						pAvatar->mfStartAnimationOnce(WALKSEQ);
						
						const WadThingType bumpResult = pAvatar->mfCheckNewPosition2(&tp, pAvatar->mfGetMarchRate());
						if(bumpResult == iNOTHING)
						{
							pAvatar->FaceTo(pAvatarTarget->mfX(), pAvatarTarget->mfY());
							pAvatar->attrib.IsFlying = TRUE;
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
			else
			{
				LONG Angle;
				LONG MythingBumped;
				FIXED_VECTOR_3D tp;
				LONG MoveAngle;
				
				// Chase the Target.
				
				// GWP If we hit something do we care?
				// GWP maybe we should try a slightly offset angle from 
				// GWP the first try and try again.
				
				pAvatar->mfStartAnimationOnce(MARCHSEQ);
				pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
		   		pAvatar->mfSetLoopingSnd(waveId, 9); 

				const WadThingType bump = pAvatar->mfFlyTowardWithBump(
				                            distance,
											pAvatarTarget->mfX(),
						                    pAvatarTarget->mfY(),
											pAvatarTarget->mfZ() + ((distance > FlyDownDistance) ? flyHeight : 0),
						                    FlyDownDistance,
											pAvatar->mfGetMarchRate(),
						                    (SHORT)((TRUE == pAvatar->mfStayInMonsterBox()) 
									                 ? CHECKLINE_MONSTER : 0),
						                    &Angle,
											&MythingBumped);		
				switch(bump)
				{			 
				case iNOTHING:
				case iSHALLOW_WATER:
				case iDEEP_WATER:
				case iLAVA:
				case iACID:
				case iHOLE:
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
						pAvatar->mfStartAnimationOnce(MARCHSEQ);
						pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
						pAvatar->mfSetLoopingSnd(waveId, 9); 

						pAvatar->Status = AI_SEARCH;
						if(pAvatar->attrib.IsFollowPath == TRUE)
						{
							//pAvatar->FaceTo(pAvatar->fFollowPath.oldP.x, pAvatar->fFollowPath.oldP.y);
							pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PATH);
							pAvatar->DoAI(CAvatar::AI_INIT);
							break;
						}

						//pAvatar->attrib.IssuedChallange = FALSE;
						pAvatar->attrib.IsFlying = TRUE;
						pAvatar->mfMoveToXYZA((pAvatar->mfX()+PLAYER_INT_VAL(tp.dx)),
											  (pAvatar->mfY()+PLAYER_INT_VAL(tp.dy)),
											  pAvatar->mfZ() + tp.dz,		//Hack 
											   (pAvatar->mfAngle()+128));

					}
					else
					{
						MoveAngle=AngleFromPoint2(pAvatar->mfX(), pAvatar->mfY(), pAvatarTarget->mfX(), pAvatarTarget->mfY(), 0);

						Rotate((POINT*)&tp, MoveAngle+pAvatar->dAngle);

						pAvatar->mfStartAnimationOnce(MARCHSEQ);
						pAvatar->mfQueueSnd(waveId, waveRange, WingFlappingFrame);
						pAvatar->mfSetLoopingSnd(waveId, 9); 

						
						const WadThingType bumpResult = pAvatar->mfCheckNewPosition2(&tp, pAvatar->mfGetMarchRate());
						if(bumpResult == iNOTHING)
						{
							pAvatar->FaceTo(pAvatarTarget->mfX(), pAvatarTarget->mfY());
							pAvatar->attrib.IsFlying = TRUE;
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

