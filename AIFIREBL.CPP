/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aifirebl.cpp  -
   Author:   Lan Zhou

   ========================================================================

   Contains the following general functions:


   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <limits.h>

#include "AVATAR.HXX"
#include "EXPLEVEL.HXX"
#include "SOUND.HXX"
#include "SCENE.HXX"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define MAX_TRAVEL_DISTANCE 3000
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
static LONG ExplosionHeight = 87;

static void BlastTheNeighbors(CAvatar * const pCastAvatar,
							  CAvatar * const hAvatar,
							  THINGTYPE const KillType,
							  LONG const Damage);
/* ========================================================================
   Function	- FireBall
   Description - AI function to bounce off the monster box looking for
   			     the "player" to attack.
   Returns	 - Current state
   ======================================================================== */
void CAvatar::FireBall (CAvatar *pAvatar, CAvatar::AISTATUS Status)
{
	switch( Status )
	{
	case AI_INIT:
		   	{

			pAvatar->Status = AI_ATTACK;
		   	pAvatar->attrib.IssuedChallange = FALSE;
		   	pAvatar->attrib.IsFlying = TRUE;
		   	pAvatar->fDamageFlag = 1;
			switch (pAvatar->mfType())
			{
		 	case ARROWS:
				break;
		 	case MAGIC_MISSILE_1:
				AddSndObj( SND_PLASMA_STREAK1, SND_PLASMA_STREAK_TOTAL, pAvatar->ThingIndex);
				break;
			case PLASMA_BALL_1:
				AddSndObj( SND_PLASMA_BALL1_1, SND_PLASMA_BALL1_TOTAL, pAvatar->ThingIndex);
				break;
			case CHAIN_LIGHTNING_2:
			case LIGHTNING_1:
				AddSndObj( SND_PLASMA_SHOT1, SND_PLASMA_SHOT_TOTAL, pAvatar->ThingIndex);
				break;
			case VAPORIZE_1:
				AddSndObj( SND_VAPORIZE1, SND_VAPORIZE_TOTAL, pAvatar->ThingIndex);
				break;
			case FIREBALL_1:
			default:
				AddSndObj(SND_THROW_FIREBALL1, SND_THROW_FIREBALL_TOTAL, pAvatar->ThingIndex);
				break;
			}
		
		   	pAvatar->mfStartAnimationLoop(STANDSEQ);
			pAvatar->fFireBall.Movement.dx = 0;
			pAvatar->fFireBall.Movement.dy = pAvatar->mfGetMarchRate();
			pAvatar->fFireBall.Movement.dz = 0;

			pAvatar->fFireBall.Home.x = pAvatar->mfX();
			pAvatar->fFireBall.Home.y = pAvatar->mfY();
			pAvatar->fFireBall.Home.z = pAvatar->mfZ();

			pAvatar->fFireBall.Target.x = 0;
			pAvatar->fFireBall.Target.y = MAX_TRAVEL_DISTANCE;
			pAvatar->fFireBall.Target.z = 0;

			Rotate((POINT *)(&(pAvatar->fFireBall.Target)), pAvatar->mfAngle());
			
			pAvatar->fFireBall.Target.x += pAvatar->fFireBall.Home.x;
			pAvatar->fFireBall.Target.y += pAvatar->fFireBall.Home.y;
			
			// what if pAvatar->hEnemy is fERROR
			if(pAvatar->hEnemy != fERROR)
			{
				CAvatar * pAvatarTarget = (CAvatar *) BLKPTR(pAvatar->hEnemy);
				const LONG distance = aprox_dist(pAvatar->mfX(), pAvatar->mfY(),
										pAvatarTarget->mfX(), pAvatarTarget->mfY());
			 	
				LONG deltaZ;
				
				if(pAvatarTarget->mfHeight() >= pAvatar->mfHeight())
					deltaZ = pAvatarTarget->mfZ() +
					         ((pAvatarTarget->mfHeight()/2) - (pAvatar->mfHeight()/2)) - pAvatar->mfZ(); 						
				else
					deltaZ = pAvatarTarget->mfZ() - pAvatar->mfZ();						
					
				if(distance!=0)
					pAvatar->fFireBall.Target.z = MAX_TRAVEL_DISTANCE * deltaZ / ABS(distance);
				else
				{
					pAvatar->mfMoveToXYZA(pAvatar->mfX(), pAvatar->mfY(),
										pAvatarTarget->mfZ()+((ExplosionHeight-pAvatarTarget->mfHeight())/2),
										pAvatar->mfAngle());
				}
			}
			pAvatar->fFireBall.Target.z += pAvatar->fFireBall.Home.z;
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
	
	case AI_ROTATE_TO_CAMERA:
			pAvatar->mfRotateToCamera();
			break;
			
	case AI_DEAD:
			if (pAvatar->attrib.DiedLastFrame)
			{
//				pAvatar->mfSpewAll();
				pAvatar->attrib.DiedLastFrame=FALSE;
			}

			if ((pAvatar->CurSequence == ATTACK1SEQ &&
			     pAvatar->mfTestSequenceDone()) &&
			     pAvatar->CurSequence != DEADSEQ)
			{
				pAvatar->mfPlayAnimationOnce(DEADSEQ);
				pAvatar->attrib.DiedLastFrame=TRUE;
			}
			break;
			
	case AI_CONSUME:
			 // fire ball disappears
			// GEH if (pAvatar->mfGetArtSequence() == EXPIRESEQ &&
			// GEH      pAvatar->mfTestSequenceDone())
			if (pAvatar->mfType() == ARROWS ||
				pAvatar->mfTestSequenceDone())
			{
			   	pAvatar->fDamageFlag = 0;
				pAvatar->DeleteAvatar(pAvatar->hThis);
			}
			break;
			
	case AI_ATTACK:
			{
			LONG distanceTraveled = aprox_dist(pAvatar->fFireBall.Home.x, pAvatar->fFireBall.Home.y,
			 					   pAvatar->mfX(), pAvatar->mfY());
			LONG FloorHeight;
			LONG CeilingHeight;
			LONG Special;
			pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
			
			if(distanceTraveled >= MAX_TRAVEL_DISTANCE - 10)
			{
				if(pAvatar->mfZ() > FloorHeight + (ExplosionHeight - pAvatar->mfHeight())/2)
					pAvatar->mfMoveToXYZA(pAvatar->mfX(), pAvatar->mfY(),
										pAvatar->mfZ()-((ExplosionHeight-pAvatar->mfHeight())/2),
										pAvatar->mfAngle());
				pAvatar->Status=AI_CONSUME;
				if (pAvatar->mfType() != ARROWS)
				{
					pAvatar->mfPlayAnimationOnce(EXPIRESEQ);
					AddSndObj(SND_FIREBALL_EXPLODE1, SND_FIREBALL_EXPLODE_TOTAL, pAvatar->ThingIndex);
				}
			}
			else
			{
				LONG Angle;
				PLAYER TempPlayer;
				FIXED_VECTOR_3D NewVector, OldVector;
				LONG const Rate=pAvatar->mfGetMarchRate();
				WadThingType Result = iNOTHING;
				LONG BumpDistance;
				
				LONG BumpIndex;
				
				if (pAvatar->hEnemy == fERROR)
				{
					pAvatar->MoveToward(
						pAvatar->fFireBall.Target.x,
						pAvatar->fFireBall.Target.y,
						pAvatar->fFireBall.Target.z,
						Rate);
					BumpIndex = 0;
				}
				else
				{
					CAvatar const * const pAvatarTarget = (CAvatar const *const ) BLKPTR(pAvatar->hEnemy);
					
					// Hit'em in the middle.
					pAvatar->fFireBall.Target.z = pAvatarTarget->mfZ() + ((pAvatarTarget->mfHeight()/2) - (pAvatar->mfHeight()/2));
					pAvatar->fFireBall.Target.x = pAvatarTarget->mfX();
					pAvatar->fFireBall.Target.y = pAvatarTarget->mfY();
					
					BumpIndex = pAvatarTarget->ThingIndex;
				}
				
				LONG const dz = pAvatar->fFireBall.Target.z - pAvatar->mfZ();
				LONG const dx = pAvatar->fFireBall.Target.x - pAvatar->mfX();
				LONG const dy = pAvatar->fFireBall.Target.y - pAvatar->mfY();
				
				LONG const distanceToTarget = aprox_dist(pAvatar->mfX(), pAvatar->mfY(),
										pAvatar->fFireBall.Target.x, pAvatar->fFireBall.Target.y);
				pAvatar->mfStartAnimationOnce(STANDSEQ);

				pAvatar->FaceTo(pAvatar->fFireBall.Target.x, pAvatar->fFireBall.Target.y);
				pAvatar->mfConvertPositionToPlayer(TempPlayer);
				if (ABS(distanceToTarget) < Rate)
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
					NewVector.dx = ((dx * Rate)/distanceToTarget);
					NewVector.dy = ((dy * Rate)/distanceToTarget);
					NewVector.dz = dz*Rate/ABS(distanceToTarget);
				}
					
				NewVector.dx <<= PLAYER_FIXEDPT;
				NewVector.dy <<= PLAYER_FIXEDPT;
				OldVector = NewVector;	// struct copy.
		
				if (distanceToTarget < 100)
				{
					// fake the bump
					TempPlayer.bump = iOBJECT;
					TempPlayer.BumpIndex = BumpIndex;
				}
				else
				if (pAvatar->mfIsVisible())
				{
					CheckBump(&TempPlayer, &NewVector, Rate);
				}
				
				if(TempPlayer.bump == iOBJECT)
				{
					// if we have a target enemy, avoid all other avatars to
					// get to this one
					if (pAvatar->hEnemy != fERROR)
					{
						if(TempPlayer.BumpIndex == BumpIndex)
						{
							if(pAvatar->mfZ() > FloorHeight + (ExplosionHeight - pAvatar->mfHeight())/2)
								pAvatar->mfMoveToXYZA(pAvatar->mfX(), pAvatar->mfY(),
													pAvatar->mfZ()-((ExplosionHeight-pAvatar->mfHeight())/2),
													pAvatar->mfAngle());

							//if, for some reason, hEnemy has become fERROR, explode!
							if (pAvatar->hEnemy == fERROR)
							{
							 	pAvatar->Status = AI_CONSUME;
								if (pAvatar->mfType() != ARROWS)
								{
							 		pAvatar->mfPlayAnimationOnce(EXPIRESEQ);
								 	AddSndObj(SND_FIREBALL_EXPLODE1, SND_FIREBALL_EXPLODE_TOTAL, pAvatar->ThingIndex);
								}
							}
							else
							{
								CAvatar * const pCastAvatar = (CAvatar * const) BLKPTR(pAvatar->fFireBall.hWhoCast);
								
								if (pAvatar->mfType() == FIREBALL_1 )
								{
									// Now blast everyone nearby.
									BlastTheNeighbors(pCastAvatar,
													  pAvatar,
													  pAvatar->mfType(),
													  pAvatar->fDamage);

									pAvatar->Status = AI_CONSUME;
									if (pAvatar->mfType() != ARROWS)
									{
										pAvatar->mfPlayAnimationOnce(EXPIRESEQ);
										AddSndObj(SND_FIREBALL_EXPLODE1, SND_FIREBALL_EXPLODE_TOTAL, pAvatar->ThingIndex);
									}
								}
								else if (pAvatar->mfType() == MAGIC_MISSILE_1 )
								{
									// no save vs Magic Missiles
									CAvatar * const pEnemy = (CAvatar * const) BLKPTR(pAvatar->hEnemy);
									pCastAvatar->mfInflictMagicDamage(pEnemy, pAvatar->mfType(), pAvatar->fDamage);


									pAvatar->Status = AI_CONSUME;
									if (pAvatar->mfType() != ARROWS)
									{
										pAvatar->mfPlayAnimationOnce(EXPIRESEQ);
										AddSndObj(SND_FIREBALL_EXPLODE1, SND_FIREBALL_EXPLODE_TOTAL, pAvatar->ThingIndex);
									}
								}
								else if (pAvatar->mfType() == CHAIN_LIGHTNING_2)
								{	
									//we have to inflict damage, then subtract
									//some damage, then find a new target and
									//go after him or her
									
									CAvatar * const pEnemy = (CAvatar * const) BLKPTR(pAvatar->hEnemy);
									SHORT hOldEnemy=pAvatar->hEnemy;

									//inflicts same type of damage as lightning
									pCastAvatar->mfInflictMagicDamage(pEnemy, LIGHTNING_1, pAvatar->fDamage);
									
									pAvatar->fDamage-=((rand()%6)+1);
									if (pAvatar->fDamage<=0)
									{
										pAvatar->Status = AI_CONSUME;
										if (pAvatar->mfType() != ARROWS)
										{
											pAvatar->mfPlayAnimationOnce(EXPIRESEQ);
											AddSndObj(SND_FIREBALL_EXPLODE1, SND_FIREBALL_EXPLODE_TOTAL, pAvatar->ThingIndex);
										}
									}
				
									else  //time to get a new target
									{
										LONG i=0;
										ULONG MinDist=ULONG_MAX; //largest 32 bit number
										LONG  MinDistIdx=-1;
										SCENE const * const pScene=(SCENE const * const)BLKPTR(SCENE_MGR::hCurrentScene);
										
							
										for (i = 0;i<MAX_AVATARS;i++)
										{
											if (pScene->Avatars[i]!=fERROR)// && pScene->Avatars[i]!=hOldEnemy)
											{
												CAvatar* pTestAva=(CAvatar*)BLKPTR(pScene->Avatars[i]);
												//test to make sure it's not
												//another fireball or something
												//flying around
												if (pTestAva->mfType() >=FIRST_PROJECTILE_EFFECT && pTestAva->mfType()<=LAST_PROJECTILE_EFFECT)
													continue;
			

												if (pTestAva->Status==AI_DEAD)
													continue;
												
												
												
												
												//now make sure it doesn't come back and hit the party										
												ADVENTURER_TEAM_ITOR AdvItor;
												for (AdvItor = ADVENTURER::begin(); AdvItor != ADVENTURER::end();AdvItor++)
													if (*AdvItor==pScene->Avatars[i])
														break;

												if (!(AdvItor != ADVENTURER::end()) || *AdvItor==pScene->Avatars[i])
													continue;
												
												//do I have to worry about other types?									
			
												LONG const TestDist=aprox_dist(pTestAva->mfX(),pTestAva->mfY(),pAvatar->mfX(),pAvatar->mfY());
												if (TestDist < MinDist)
												{
													MinDist=TestDist;
													MinDistIdx=i;
												}
											}
										}

										//if the new target is just too far away...
										if (MinDist > MAX_TRAVEL_DISTANCE)
										{
											pAvatar->Status = AI_CONSUME;
											if (pAvatar->mfType() != ARROWS)
											{
												pAvatar->mfPlayAnimationOnce(EXPIRESEQ);
												AddSndObj(SND_FIREBALL_EXPLODE1, SND_FIREBALL_EXPLODE_TOTAL, pAvatar->ThingIndex);
											}
										}
							
										if (MinDistIdx!=-1)
										{
											pAvatar->hEnemy=pScene->Avatars[MinDistIdx];
											CAvatar * const pNewEnemy = (CAvatar * const) BLKPTR(pAvatar->hEnemy);
											
											pAvatar->fFireBall.Target.z = pNewEnemy->mfZ();
											pAvatar->fFireBall.Target.x = pNewEnemy->mfX();
											pAvatar->fFireBall.Target.y = pNewEnemy->mfY();
											
											//Let it go on it's merry way
											//but let's create an explosion,
											//so it actually sounds like it
											//hits the old target
											if (pAvatar->mfType() != ARROWS)
												AddSndObj(SND_FIREBALL_EXPLODE1, SND_FIREBALL_EXPLODE_TOTAL, pAvatar->ThingIndex);
											
										}
																
									}
								}

								else
								{
									CAvatar * const pEnemy = (CAvatar * const) BLKPTR(pAvatar->hEnemy);
									if (!SaveVs(pEnemy->hPlayerStats, ST_SPELL, 0))
										pCastAvatar->mfInflictMagicDamage(pEnemy, pAvatar->mfType(), pAvatar->fDamage);
									else	

										pCastAvatar->mfInflictMagicDamage(pEnemy, pAvatar->mfType(), pAvatar->fDamage/2);


									pAvatar->Status = AI_CONSUME;
									if (pAvatar->mfType() != ARROWS)
									{
										pAvatar->mfPlayAnimationOnce(EXPIRESEQ);
										AddSndObj(SND_FIREBALL_EXPLODE1, SND_FIREBALL_EXPLODE_TOTAL, pAvatar->ThingIndex);
									}
								}
							}
							

							break;
						}
					}
					// else, there was no enemy specified;
					// nuke the first avatar we come to
					else
					{
						SCENE * const pCurrentScene = (SCENE * const) BLKPTR(SCENE_MGR::hCurrentScene);
						// check to see if this thing index is an avatar
						LONG i;
						for(i = 0; i < MAX_AVATARS; i++ )
						{
							// This is a sparsly filled array.
							if (pCurrentScene->Avatars[i] != fERROR)
							{
								CAvatar *pTestAvatar = (CAvatar *) BLKPTR(pCurrentScene->Avatars[i]);
								if(pTestAvatar->ThingIndex == TempPlayer.BumpIndex)
									break; // found him
							}
						}
			
						if(i != MAX_AVATARS)
						{
							if(pAvatar->mfZ() > FloorHeight + (ExplosionHeight - pAvatar->mfHeight())/2)
								pAvatar->mfMoveToXYZA(pAvatar->mfX(), pAvatar->mfY(),
													pAvatar->mfZ()-((ExplosionHeight-pAvatar->mfHeight())/2),
													pAvatar->mfAngle());
							
							CAvatar * const pCastAvatar = (CAvatar * const) BLKPTR(pAvatar->fFireBall.hWhoCast);
							
							if (pAvatar->mfType() == PLASMA_STREAK_1 ||
							    pAvatar->mfType() == LORES_PLASMA_STREAK_1 ||
							    pAvatar->mfType() == LIGHTNING_1 ||
							    pAvatar->mfType() == LORES_LIGHTNING_1)
							{
								CAvatar * const pEnemy = (CAvatar * const) BLKPTR(pCurrentScene->Avatars[i]);
									if (!SaveVs(pEnemy->hPlayerStats, ST_SPELL, 0))
										pCastAvatar->mfInflictMagicDamage(pEnemy, pAvatar->mfType(), pAvatar->fDamage);
									else	
										pCastAvatar->mfInflictMagicDamage(pEnemy, pAvatar->mfType(), pAvatar->fDamage/2);
							}
							else if (pAvatar->mfType() == MAGIC_MISSILE_1)
							{
								CAvatar * const pEnemy = (CAvatar * const) BLKPTR(pCurrentScene->Avatars[i]);
								pCastAvatar->mfInflictMagicDamage(pEnemy, pAvatar->mfType(), pAvatar->fDamage);
							}
							else
							{
								// Now blast everyone nearby.
								BlastTheNeighbors(pCastAvatar,
												  pAvatar,
												  pAvatar->mfType(),
												  pAvatar->fDamage);
							}
								
							pAvatar->Status = AI_CONSUME;
							if (pAvatar->mfType() != ARROWS)
							{
								pAvatar->mfPlayAnimationOnce(EXPIRESEQ);
								AddSndObj(SND_FIREBALL_EXPLODE1, SND_FIREBALL_EXPLODE_TOTAL, pAvatar->ThingIndex);
							}
							break;
						}
					}
	
					THINGTYPE const BumpType = (THINGTYPE) mythings[TempPlayer.BumpIndex].type;
						
					// Don't bump other people or magic things. (Dead fireball people.)
					if (GAME_TTYPE::mfIsMissileBlocker (BumpType))
					{	
						if(pAvatar->mfZ() > FloorHeight + (ExplosionHeight - pAvatar->mfHeight())/2)
							pAvatar->mfMoveToXYZA(pAvatar->mfX(), pAvatar->mfY(),
												pAvatar->mfZ()-((ExplosionHeight-pAvatar->mfHeight())/2),
												pAvatar->mfAngle());
						pAvatar->Status = AI_CONSUME;
						if (pAvatar->mfType() != ARROWS)
						{
							pAvatar->mfPlayAnimationOnce(EXPIRESEQ);
							AddSndObj(SND_FIREBALL_EXPLODE1, SND_FIREBALL_EXPLODE_TOTAL, pAvatar->ThingIndex);
						}	
						CAvatar * const pCastAvatar = (CAvatar * const) BLKPTR(pAvatar->fFireBall.hWhoCast);
							
						// If not a single object missle, get everyone nearby.
						if (!(pAvatar->mfType() == MAGIC_MISSILE_1 ||
						    pAvatar->mfType() == PLASMA_STREAK_1 ||
						    pAvatar->mfType() == LORES_PLASMA_STREAK_1 ||
						    pAvatar->mfType() == LIGHTNING_1 ||
						    pAvatar->mfType() == LORES_LIGHTNING_1))
						{
								// Now Blast everyone nearby.
								BlastTheNeighbors(pCastAvatar,
												  pAvatar,
												  pAvatar->mfType(),
												  pAvatar->fDamage);
						}
						break;
					}
					else
					{
						NewVector = OldVector;	// struct copy
					}
				}
					
				const WadThingType bump=CheckMove(&TempPlayer, &NewVector, 0, &Angle, &BumpDistance);
				if((bump == iFLOOR || bump == iCEILING || bump == iWALL ||
				    bump == iEDGE_OF_WORLD) ||
				   NewVector.dx != OldVector.dx ||
				   NewVector.dy != OldVector.dy ||
				   NewVector.dz != OldVector.dz)
				{
					if(pAvatar->mfZ() > FloorHeight + (ExplosionHeight - pAvatar->mfHeight())/2)
						pAvatar->mfMoveToXYZA(pAvatar->mfX(), pAvatar->mfY(),
											pAvatar->mfZ()-((ExplosionHeight-pAvatar->mfHeight())/2),
											pAvatar->mfAngle());
					pAvatar->Status = AI_CONSUME;
					if (pAvatar->mfType() != ARROWS)
					{
						pAvatar->mfPlayAnimationOnce(EXPIRESEQ);
						AddSndObj(SND_FIREBALL_EXPLODE1, SND_FIREBALL_EXPLODE_TOTAL, pAvatar->ThingIndex);
					}
				}
				else
				{
					pAvatar->mfMoveToXYZA(pAvatar->mfX() + PLAYER_INT_VAL(NewVector.dx),
								 pAvatar->mfY() + PLAYER_INT_VAL(NewVector.dy),
								 pAvatar->mfZ() + NewVector.dz,
								 pAvatar->mfAngle());
				}	
			}
			}
			break;
	}
}
	
/* ========================================================================
   Function	- BlastTheNeighbors.
   Description - In an adventure, hand out damge to the neighboring avatars.
   Returns	 -
   ======================================================================== */
static void BlastTheNeighbors(
	CAvatar * const pCastAvatar,
	CAvatar * const pAvatar,
	THINGTYPE const KillType,
	LONG const Damage)
{
	LONG const BlastDistance = 75;
	
	if (SCENE_MGR::hCurrentScene != fERROR)
	{
		LONG i;
		SCENE_AI::STATE OldSceneState;
		SCENE_AI::SCENE_TYPE OldSceneType;
		PLAYER	TempPlayer;
		
		SCENE * const pCurrentScene = (SCENE * const) BLKPTR(SCENE_MGR::hCurrentScene);
		pCurrentScene->mfGetSceneAIState(&OldSceneState,
						                 &OldSceneType);
		
		
		if (OldSceneType == SCENE_AI::BATTLE_SCENE)
		{
			if (pAvatar->hEnemy == fERROR)
				return;
				
			CAvatar * const pEnemy = (CAvatar * const) BLKPTR(pAvatar->hEnemy);
			// See if the explosion hit something next to the battle captain.
			LONG const distanceToTarget = aprox_dist(pEnemy->mfX(),
				                                  pEnemy->mfY(),
				                                  pAvatar->mfX(),
				                                  pAvatar->mfY());
			if (distanceToTarget < BlastDistance)
			{
				// In battle we just hit the battle captain.
				pCastAvatar->mfInflictMagicDamage(pEnemy, KillType, Damage);
			}
			return;
		}
			
		TempPlayer.x = pAvatar->mfX() << PLAYER_FIXEDPT;
		TempPlayer.y = pAvatar->mfY() << PLAYER_FIXEDPT;
		TempPlayer.z = pAvatar->mfZ() + 32;
		TempPlayer.p = 0;
		TempPlayer.a = pAvatar->mfAngle();
		TempPlayer.Flying = TRUE;
		TempPlayer.w = 16;
		TempPlayer.h = 16;
		TempPlayer.bump = iNOTHING;
		TempPlayer.ThingIndex = fERROR;
		
		for (i = 0; i < MAX_AVATARS; i++)
		{
			LONG BumpDistance;
			
			if (pCurrentScene->Avatars[i] == fERROR ||
			    pCurrentScene->Avatars[i] == pAvatar->hThis)
				continue;
				
			CAvatar * const pNeighbor = (CAvatar * const) BLKPTR(pCurrentScene->Avatars[i]);
			
			if(!reject( pAvatar->mfGetSector(), pNeighbor->mfGetSector()))
				continue;
			
			LONG const distance = aprox_dist(pAvatar->mfX(),
			                                  pAvatar->mfY(),
			                                  pNeighbor->mfX(),
			                                  pNeighbor->mfY());
			if (distance > BlastDistance)
				continue;
				
			LONG const AngleToNeighbor = AngleFromPoint(pAvatar->mfX(),
														pAvatar->mfY(),
					                                    pNeighbor->mfX(),
					                                    pNeighbor->mfY(),
					                                    RESOLUTION_4);
			TempPlayer.a = AngleToNeighbor;
			
			CheckLongMove(&TempPlayer,
			          AngleToNeighbor,
					  distance,
			          0,
			          pNeighbor->mfZ(),
			          &BumpDistance);
			
		    if (TempPlayer.bump == iNOTHING)
			{
				if (pNeighbor->hPlayerStats == fERROR)
				{
					// They have no stats, so ignore'm
					continue;
				}
				if (!SaveVs(pNeighbor->hPlayerStats, ST_SPELL, 0))
					pCastAvatar->mfInflictMagicDamage(pNeighbor, KillType, Damage);
				else	
					pCastAvatar->mfInflictMagicDamage(pNeighbor, KillType, Damage/2);
			}
		}
	}
}
