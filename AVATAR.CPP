
/* ========================================================================
   Copyright (c) 1990,1996	  Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: Avatar.cpp
   Author:   Craig Clayton
   ======================================================================== */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "avatar.hxx"

#include "typedefs.h"
#include "engine.h"

#include "ai_utils.h"
#include "combopts.hxx"
#include "gamemap.hxx"
#include "gmenuenm.h"
#include "thingtyp.h"
#include "playstat.hxx"
#include "playhit.hxx"
#include "invntory.hxx"
#include "special.h"
#include "handle.hxx"
#include "units.hxx"
#include "scene.hxx"

#ifdef _WINDOWS
#ifdef OLD_SOUND
#include "winsys\sndutil.h"
#include "winsys\mciutil.h"
#include "winsys\musiutil.h"
#endif
#endif

#if defined (_VC4)
#define static
#endif

#define FALL_DIVISOR 			128
#define DAMAGE_FRAME				5
#define LAVA_DAMAGE 				1
#define ACID_DAMAGE 				1
#define POISON_DAMAGE 			1

SHORT CAvatar::PlayerAvatarHandle=fERROR;
const LONG CAvatar::ActivationDistance = 1000;
const LONG CAvatar::fgTauntDelay = 50;

// [d11-05-96 JPC] Experiment for keeping dead things from clipping so badly.
#define EXTRATHINGS 0	// conform to #define in THINGS.C
#if EXTRATHINGS
extern "C" {
void AddThingToAdjacentSubsectors (LONG ThingIndex);
}
#endif

// Saves avatar ID to disk

#if 0 	// UNUSED
void CAvatar::Save(void)
{
	// Don't save the troops from a battle captain.
	if (attrib.RuntimeCreated == FALSE)
	{
		char szFileName[20] = { "scenes\\"};
		char szID[10];
		FILE *pFileWrite;
	
		strcat(szFileName, "id");
		sprintf(szID, "%ld", Id);
		strcat(szFileName, szID);
		strcat(szFileName, ".ava");
	
		pFileWrite = FileOpen(szFileName,"wb");
			
		if (pFileWrite == NULL)			// OOPS!  No file
				return;
	
		fwrite(&Version, 	sizeof(long), 1, pFileWrite);
		if (hPlayerStats != fERROR)
		{
			PTR_PLAYER_STATS pPlayerStats = (PTR_PLAYER_STATS) BLKPTR(hPlayerStats);
			pPlayerStats->mfWriteData(pFileWrite);
		}
	
		FileClose(pFileWrite);		
	}
}
#endif	// UNUSED


void CAvatar::SetAIFuncIndex(AIFUNC aif)
{
	AIFuncIndex = aif;
	
	switch (aif)
	{
		case AI_FUNC_BATTLE_CAPTAIN:
			pAvatarFunc = BattleCapt;
			break;
 		case AI_FUNC_FOLLOW_PATH:
			pAvatarFunc = FollowPath;
			break;
#if 0 // UNUSED
  		case AI_FUNC_FOLLOW_ID:
 			pAvatarFunc = FollowId;
			break;
		case AI_FUNC_FIND_TYPE:
			pAvatarFunc = FindType;
			break;
		case AI_FUNC_GUARD_POINT:
			pAvatarFunc = GuardPt;
			break;
		case AI_FUNC_STAND_STILL:
			pAvatarFunc = StandStill;
			break;
		case AI_FUNC_DEATH_SEQUENCE:
			pAvatarFunc = DeathSequence;
			break;
		case AI_FUNC_PATH_LEADER:
			pAvatarFunc = PathLeader;
			break;
		case AI_FUNC_PATH_ATTACK:
			pAvatarFunc = PathAttack;
			break;
#endif
		case AI_FUNC_PING_PONG:
			pAvatarFunc = PingPong;
			break;
		case AI_FUNC_FOLLOW_PLAYER:
			pAvatarFunc = FollowPlayer;
			break;
		case AI_FUNC_FOLLOW_BTLCAP:
			pAvatarFunc = FollowBtlCap;
			break;
		case AI_FUNC_SPIDERS:
			pAvatarFunc = Spiders;
			break;
 		case AI_FUNC_HARPIES:
 			pAvatarFunc = Harpies;
 			break;
		case AI_FUNC_WYVERNS:
			pAvatarFunc = Wyverns;
			break;
 		case AI_FUNC_HELLHOUND:
 			pAvatarFunc = HellHounds;
 			break;
		case AI_FUNC_FIREBALL:
			pAvatarFunc = FireBall;
			break;
		case AI_FUNC_SPECTRE:
			pAvatarFunc = Spectres;
			break;

		case AI_FUNC_STUNNED:
			pAvatarFunc = Stunned;
			break;

		case AI_FUNC_GARGOYLES:
			pAvatarFunc = Gargoyles;
			break;
		
		case AI_FUNC_HORSE:
			pAvatarFunc = Horse;
			break;

		case AI_FUNC_NULL:
		default:
			pAvatarFunc = 0;
			break;
	}
}

#if 0 // UNUSED
/* ========================================================================
   Function	- DeathSequence
   Description - AI Proc to kill off the avatar
   Returns	 - Current state
   ======================================================================== */

void CAvatar::DeathSequence (CAvatar *pAvatar, CAvatar::AISTATUS )
{
	if (pAvatar->mfTestSequenceDone())
	{
		// If there are people following me... release them.
		
		
		
		// Actually we may want to remove the body sometime in the future.
		
		// Hey! He's Dead! Dead men don't move!
		pAvatar->SetAIFuncIndex(AI_FUNC_NULL);
	}
}
#endif	// UNUSED

#if 0 // UNUSED
/* ========================================================================
   Function	- FollowId
   Description - AI Proc to follow another avatar around
   Returns	 - Current state
   ======================================================================== */

void CAvatar::FollowId (CAvatar *pAvatar, CAvatar::AISTATUS Status)
{
	LONG	index;
	PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	
	switch( Status )
	{
	case AI_INIT:
		pAvatar->Status = AI_RUNNING;
		pAvatar->mfStartAnimationLoop(WALKSEQ);
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
		// If I'm following someone else around, send him the message.
		// otherwise I'll take it as I'm lost.
		
		pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
		index = pCurrentScene->mfAvatarIndex(pAvatar->fFollowId.Id);
		
		if (index < MAX_AVATARS && index > 0)
		{
			//GEH SetBlockAttr(pCurrentScene->Avatars[index], LOCKED, LOCKED);
			CAvatar *pLeaderAvatar = (CAvatar *) BLKPTR(pCurrentScene->Avatars[index]);
			
			pLeaderAvatar->DoAI(AI_BEGIN_LISTEN);
			
			//GEH ClrLock(pCurrentScene->Avatars[index]);
			
			// I could tell myself to pause but the leader should decide that.
		}
		else
		{
			// I can't find my leader so I guess I'll listen up.
			pAvatar->mfBeginListen();
		}
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
	
	case AI_ATTACK:
		{
			index = pCurrentScene->mfAvatarIndex(pAvatar->fFollowId.Id);
			CAvatar *pAvatar = (CAvatar *) BLKPTR(pCurrentScene->Avatars[index]);
			
			// melee
			if(index != MAX_AVATARS) // the guy I want to follow is missing
			{
				pAvatar->FaceTo(pAvatar->mfX(), pAvatar->mfY());
				pAvatar->mfStartAnimationOnce(ATTACK1SEQ);
			}
			else
			{
				pAvatar->mfStartAnimationOnce(STANDSEQ);
			}
		}
		break;
			
	case AI_MOVING:
	default:
		pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
		const LONG oldX = pAvatar->mfX();
		const LONG oldY = pAvatar->mfY();
		
		index = pCurrentScene->mfAvatarIndex(pAvatar->fFollowId.Id);
		CAvatar *pAvatar = (CAvatar *) BLKPTR(pCurrentScene->Avatars[index]);
		if(index != MAX_AVATARS) // the guy I want to follow was found.
		{
			LONG tx = pAvatar->fFollowId.NewX;
			LONG ty = pAvatar->fFollowId.NewY;
			
			//tx += random(5) - 2;
			//ty += random(5) - 2;
			
			const LONG dist = pAvatar->MoveToward(tx, ty, 4, pAvatar->fFollowId.Rate);
			if (dist > 0)
				pAvatar->mfStartAnimationOnce(STANDSEQ);
			else
				pAvatar->mfStartAnimationOnce(WALKSEQ);
			
			
			pAvatar->mfSetAngle(pAvatar->fFollowId.NewA);
		}
		else
		{
			// Wait for next command.
			pAvatar->mfBeginPause();
		}
		break;
	}
}
#endif	// UNUSED

#if 0 // UNUSED
/* ========================================================================
   Function	- FindType
   Description - AI Proc to find the nearest avatar of type
   Returns	 - Current state
   ======================================================================== */

void CAvatar::FindType (CAvatar *pAvatar, CAvatar::AISTATUS Status)
{
	LONG	index;
	CAvatar *pLeaderAvatar;
	PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
		
	switch( Status )
	{
	case AI_INIT:
			pAvatar->Status = AI_SEARCH;
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
		
	case AI_SEARCH:
			{
			// find the nearest unengaged enemy
			//GEH!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//    This is Wrong
			//SHORT Id = pAvatar->FindUnengaged( fERROR, pAvatar->fFindType.TargetType );
			
			// GEH if( MAX_AVATARS == fERROR )
			// GEH {
			// GEH		// no more of this type to find
			// GEH		pAvatar->Status = AI_PAUSED;
			// GEH		pAvatar->fFindType.Id = fERROR;
			// GEH}
			// GEHelse
			// GEH{
			// GEH		//pAvatar->fFindType.Id = Id;
			// GEH		//pAvatar->Status = AI_MOVING;
			// GEH}
			}
			break;
			
	case AI_ATTACK:
			// melee
			index = pCurrentScene->mfAvatarIndex(pAvatar->fFindType.Id);
			pLeaderAvatar = (CAvatar *) BLKPTR(pCurrentScene->Avatars[index]);
			
			if(index != MAX_AVATARS)
			{
					pAvatar->MoveToward(pLeaderAvatar->mfX(),
								pLeaderAvatar->mfY(),
								10 ,
								pAvatar->mfGetMarchRate());
			}
			else
			{
					// the guy I want to follow is missing
					pAvatar->Status = AI_SEARCH;
					pLeaderAvatar->fFindType.Control &= ~AIC_ENGAGED;
			}
			break;
			
	case AI_MOVING:
			// seek and engage the enemy
			index = pCurrentScene->mfAvatarIndex(pAvatar->fFindType.Id);
	
			if(index != MAX_AVATARS) // the guy I want to follow is missing
			{
				pLeaderAvatar = (CAvatar *) BLKPTR(pCurrentScene->Avatars[index]);
				LONG h = pAvatar->MoveToward( pLeaderAvatar->mfX(),
							pLeaderAvatar->mfY(),
							10 ,
							pAvatar->mfGetMarchRate());
				if( h < 12 )
				{
						pAvatar->Status = AI_ATTACK;
						pLeaderAvatar->fFindType.Control |= AIC_ENGAGED;
				}
			}
			else
			{
					pAvatar->Status = AI_SEARCH;
			}
			break;
			
	default:
			break;
	}
}
#endif // UNUSED

#if 0 // UNUSED
/* ========================================================================
   Function    - GuardPt
   Description - Stand at Pt, only move for camera
   Returns     - Current state
   ======================================================================== */
void CAvatar::GuardPt (CAvatar *pAvatar, CAvatar::AISTATUS Status)
{
		
		switch( Status )
		{
		case AI_INIT:
				pAvatar->Status = AI_RUNNING;
				// Remember our original angle.
				pAvatar->fGuardPt.GuardPositionAngle = pAvatar->mfAngle();
				pAvatar->fGuardPt.Wave = 0;
				break;
				
		case AI_RELEASE:
				pAvatar->Status = AI_INIT;
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
		
		default:
				{
				BOOL	Move = FALSE;
				LONG	TargetX, TargetY;
				LONG	distance;
				LONG	DectionDistance = 200;

				// Am I at the secondary point?
				if (pAvatar->mfX() != pAvatar->fGuardPt.X &&
				    pAvatar->mfY() != pAvatar->fGuardPt.Y)
				{
					// If so make the dection distance a bit longer.
					DectionDistance += 200;
				}
				
				if (pAvatar->mfIsVisible())
				{
					// First check how far away the camera is.
					distance = dist(pAvatar->mfX(),
									pAvatar->mfY(),
									PLAYER_INT_VAL(player.x),
									PLAYER_INT_VAL(player.y));
	
					if (distance < DectionDistance)
					{
						// The camera is close, move out of the way.
						Move = TRUE;
						TargetX = pAvatar->fGuardPt.StandAsideX;
						TargetY = pAvatar->fGuardPt.StandAsideY;
						
						// Face the Camera.
						pAvatar->FaceTo(PLAYER_INT_VAL(player.x), PLAYER_INT_VAL(player.y));
#ifdef _WINDOWS
#ifdef OLD_SOUND
						// if standing on my guard point when I see you, say hi
						if (
							pAvatar->fGuardPt.X == pAvatar->mfX() &&
							pAvatar->fGuardPt.Y == pAvatar->mfY()
							)
						{
							pfVoiceResp( pAvatar->mfType() );
						}
							
						// If I'm a dog Bark at the camera when really close!
						if (pAvatar->mfType() == DOG )
						{
							if ( distance < 150 )
							{
								if ( pAvatar->fGuardPt.Wave == 0)
								{
									pAvatar->fGuardPt.Wave = (SHORT) (43 + (pAvatar->Id % 7));
									PlayWave(pAvatar->fGuardPt.Wave, TRUE);
								}
							}
							else
							{
								if ( pAvatar->fGuardPt.Wave != 0)
								{
									PauseWave(pAvatar->fGuardPt.Wave);
									pAvatar->fGuardPt.Wave = 0;
								}
							}
						}
#endif
#endif
					}
				}
				else
				{
					// Is anyone else near?
					LONG d;
					PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
					SHORT hAvatar = fERROR;
					distance = 9999;
					
					// check the list of avatars for the closest one not on Guard Duty.
					for(LONG i=0;i<MAX_AVATARS;++i)
					{
						if (pCurrentScene->Avatars[i] != fERROR)
						{
							CAvatar *pLeaderAvatar = (CAvatar *) BLKPTR(pCurrentScene->Avatars[i]);
							
							if( pLeaderAvatar->hThis == pAvatar->hThis )	// Check for self.
								continue;
					
							if (pLeaderAvatar->GetAIFuncIndex() == AI_FUNC_GUARD_POINT)
								continue;
							
							// GWP I suppose Harpie and Spiders could be guards for evil things...
							// GWP So we need to keep track of who is a friend of who.
							// GWP knowing that it is runtime game dependent.
							
							if (pLeaderAvatar->mfIsMonster())
							{
								continue;
							}
							
							d = aprox_dist(
								pAvatar->mfX(),
								pAvatar->mfY(),
								pLeaderAvatar->mfX(),
								pLeaderAvatar->mfY() );
							
							if(d < distance)
							{
								distance = d;
								hAvatar = pLeaderAvatar->hThis;
							}
						}
					}
					
					if( hAvatar != fERROR ) // Someone found.
					{
						if (distance < DectionDistance )
						{
							// Halt, who goes there!
							CAvatar * pNearAvatar = (CAvatar *) BLKPTR (hAvatar);
							
							// Face the Approaching Avatar.
							pAvatar->FaceTo(pNearAvatar->mfX(), pNearAvatar->mfY());
							
							Move = TRUE;
							TargetX = pAvatar->fGuardPt.StandAsideX;
							TargetY = pAvatar->fGuardPt.StandAsideY;
						}
					}
					
					if (Move == FALSE)
					{
						// Am I standing at the point I'm supposed to Guard?
						distance = dist (pAvatar->fGuardPt.X,
										 pAvatar->fGuardPt.Y,
										 pAvatar->mfX(),
										 pAvatar->mfY());
						
						if (distance > 0)
						{
							Move = TRUE;
							TargetX = pAvatar->fGuardPt.X;
							TargetY = pAvatar->fGuardPt.Y;
						}
						else
						{
							pAvatar->mfSetAngle(pAvatar->fGuardPt.GuardPositionAngle);
						}
					}
				}
				
				if (Move == TRUE)
				{
					LONG dx = (TargetX - pAvatar->mfX());
					LONG dy = (TargetY - pAvatar->mfY());
					
					if( abs(dx) > pAvatar->mfGetWalkRate() )
					{
						if ( dx < 0)
						{
							dx = -pAvatar->mfGetWalkRate();
						}
						else
						{
							dx = pAvatar->mfGetWalkRate();
						}
					}
					
					if( abs(dy) > pAvatar->mfGetWalkRate() )
					{
						if ( dy < 0)
						{
							dy = -pAvatar->mfGetWalkRate();
						}
						else
						{
							dy = pAvatar->mfGetWalkRate();
						}
					}
				
					if (dx == 0 && dy == 0)
					{
						// In case the Rate got set to Zero, don't run in place.
						pAvatar->mfStartAnimationLoop(STANDSEQ);
					}
					else
					{
						pAvatar->mfStartAnimationLoop(WALKSEQ);
						pAvatar->mfMoveTo( pAvatar->mfX() + dx,
										  pAvatar->mfY() + dy );
					}
				}
				else
				{
					pAvatar->mfStartAnimationLoop(STANDSEQ);
				}
				}
				break;
		}
}
#endif // UNUSED

#if 0 // UNUSED
/* ========================================================================
   Function	- StandStill
   Description - Stand Still, don't move!
   Returns	 - Current state
   ======================================================================== */
void CAvatar::StandStill (CAvatar *pAvatar, CAvatar::AISTATUS Status)
{
		
		switch( Status )
		{
		case AI_INIT:
				pAvatar->mfStartAnimationLoop(STANDSEQ);
				pAvatar->Status = AI_RUNNING;
				break;
				
		case AI_RELEASE:
				pAvatar->Status = AI_INIT;
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
		
		default:
				break;
		}
}
#endif // UNUSED

#if 0 // UNUSED
/* ========================================================================
   Function	- PathLeader
   Description - AI function to follow a point path from a file on disk
   Returns	 - Current state
   ======================================================================== */

void CAvatar::PathLeader (CAvatar *pAvatar, CAvatar::AISTATUS Status)
{
	switch( Status )
	{
	case AI_INIT:
		if (pAvatar->fPathLeader.FollowPath.PathCount > 0)
		{
			LONG X;
			LONG Y;
			LONG Z;
			LONG A;
			
			pAvatar->fPathLeader.FollowPath.mfGetCurrentPt(X, Y, Z, A);
			
			pAvatar->Status = AI_MOVING;

			pAvatar->mfMoveToXYZA( X, Y, Z, A );
			
			pAvatar->mfStartAnimationLoop( WALKSEQ);
		}
		else
		{
			pAvatar->mfStartAnimationLoop( STANDSEQ);
		}
		pAvatar->fPathLeader.Leader.mfInit(pAvatar);
		break;
			
	case AI_RELEASE:
		pAvatar->Status = AI_INIT;
		break;
			
	case AI_MOVING:
		{
		// only x and y work right now
		LONG X;
		LONG Y;
		LONG Z;
		LONG A;
		
		pAvatar->fPathLeader.FollowPath.mfGetCurrentPt(X, Y, Z, A);
		
		if( ( pAvatar->mfX() == X ) &&
			( pAvatar->mfY() == Y ) )
		{
				// You've made it to the new location, get the next
				// requested destination.
				pAvatar->fPathLeader.FollowPath.mfGetNextPt(X, Y, Z, A);
		}
		
		// if not at destination, start moving in that direction
		LONG dx = (X - pAvatar->mfX());
		LONG dy = (Y - pAvatar->mfY());
		
		const LONG h = dist(
				pAvatar->mfX(),
				pAvatar->mfY(),
				X,
				Y );
		if(!h)
		{
			// GWP The guy is lost. So just stop him.
			// GWP Maybe the data is bad...
			pAvatar->Status = AI_BEGIN_PAUSE;
			break;
		}
		
		if( abs(dx) > pAvatar->mfGetWalkRate() )
		{
			dx = dx * pAvatar->mfGetWalkRate() / h;
		}
		
		if( abs(dy) > pAvatar->mfGetWalkRate() )
		{
			dy = dy * pAvatar->mfGetWalkRate() / h;
		}
		
		// check for a bump at the target coords
		while( iNOTHING != pAvatar->mfCheckNewPosition( dx, dy, pAvatar->mfGetWalkRate()))
		{
			dx += random(pAvatar->mfGetWalkRate()*2) - pAvatar->mfGetWalkRate();
			dy += random(pAvatar->mfGetWalkRate()*2) - pAvatar->mfGetWalkRate();
		}
		
		//pAvatar->FaceTo(pAvatar->mfX() + dx,
		//			   pAvatar->mfY() + dy );
					
		pAvatar->mfMoveToXYZA( pAvatar->mfX() + dx,
						  pAvatar->mfY() + dy, Z, A);
						
		// now update anyone following me
		pAvatar->fPathLeader.Leader.mfRecordTrail(pAvatar);
		pAvatar->fPathLeader.Leader.mfMoveFollowers(pAvatar);
		}
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
	
	default:
			pAvatar->Status = AI_PAUSED;
	}
}
#endif	// UNUSED


/* ========================================================================
   Function	- MoveToward
   Description - Move to withing BUMP distance from a point by my Rate amount
   Returns	 - distance from point
   ======================================================================== */

LONG CAvatar::MoveToward(
		LONG	lTargetX,
		LONG	lTargetY,
		LONG	Bump,
		LONG	Rate
)
{
	LONG dx = lTargetX - mfX();
	LONG dy = lTargetY - mfY();
	LONG h = dist( mfX(), mfY(), lTargetX, lTargetY );
			
	if(abs(h) > Bump)
	{
		LONG NewX;
		LONG NewY;
		
		if (h > 0)
			h -= Bump;
		else
			h += Bump;
		
		FaceTo(lTargetX, lTargetY );
		
		if (Bump > 2)
		{
			// Introduce some randomness as to exactly where
			// we endup within "Bump" distance of the target.
			dx += random(Bump - 1);
			dy += random(Bump - 1);
		}
		
		if (abs(h) < Rate)
		{
			NewX = mfX() + dx;
			NewY = mfY() + dy;
		}
		else
		{
			NewX = mfX() + ((dx * Rate)/h);
			NewY = mfY() + ((dy * Rate)/h);
		}
		mfMoveTo(NewX, NewY);
	}
	else
	{
		h = 0;
	}
	
	return h;
}

#if 0	// UNUSED
/* ========================================================================
   Function	- AvatarNear
   Description - is another Avatar near this one? (use NO_THING for "Any type".
   Returns	 - Index in the AvatarArray of the closest Avatar.
   ======================================================================== */

LONG CAvatar::AvatarNear( THINGTYPE type, LONG &closestD, SHORT &hAvatar )
{
	LONG i;
	LONG ReturnVal=0;
	LONG d;
	PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	
	closestD = 9999;
	// check the list of avatars
	for(i=0;i<MAX_AVATARS;++i)
	{
		if (pCurrentScene->Avatars[i] != fERROR)
		{
			CAvatar *pAvatar = (CAvatar *) BLKPTR(pCurrentScene->Avatars[i]);
			
			if( pAvatar == this )	// Check for self.
				continue;
	
			// Check whether the type can be Anytype or is an exact match.
			if( type != NO_THING && pAvatar->mfType() != type )
				continue;
				
			d = aprox_dist(
				mfX(),
				mfY(),
				pAvatar->mfX(),
				pAvatar->mfY() );
			
			if(d < closestD)
			{
				closestD = d;
				hAvatar = pAvatar->hThis;
				ReturnVal = pAvatar->Id;
			}
		}
	}

	// Is the camera the closed Avatar? (At the moment the camera is not a type.)
	if (type == NO_THING)
	{
		d = aprox_dist( mfX(), mfY(),
			PLAYER_INT_VAL(player.x), PLAYER_INT_VAL(player.y) );
			
		if(d < closestD)
		{
			closestD = d;
			hAvatar = fERROR;
			ReturnVal = MAX_AVATARS;
		}
	}
	
	return(ReturnVal);
}
#endif // UNUSED


/* ========================================================================
   Function	- NewAvatar
   Description - Allocates memory and initializes it for an avatar.
   Returns	 - Handle to the avatar.
   ======================================================================== */
SHORT CAvatar::NewAvatar(void)
{
	SHORT hAvatar = NewLockedBlock(sizeof(CAvatar));
	
	if (hAvatar != fERROR)
	{
		LONG i;
		
		CAvatar *pAvatar = (CAvatar *) BLKPTR(hAvatar);
		
		pAvatar->hThis		= hAvatar; // Self reference pointer needed to attach to other things.
		pAvatar->attrib.RuntimeCreated	= FALSE;
		pAvatar->attrib.SceneInstance	= FALSE;
		pAvatar->attrib.Engaged			= FALSE;
		pAvatar->attrib.IsFlying		= FALSE;
		pAvatar->attrib.IsFeathered		= FALSE;
		pAvatar->attrib.CanSeeInDark	= FALSE;
		pAvatar->attrib.CanSeeInfra		= FALSE;
		pAvatar->attrib.IssuedChallange	= FALSE;
		pAvatar->attrib.IssuedTaunt		= FALSE;
		pAvatar->attrib.IsFollowPath    = FALSE;
		pAvatar->attrib.KilledBySpectre = FALSE;
		pAvatar->attrib.AttackMode = FIGHT_SEQUENCE::ATM_NONE;
		pAvatar->attrib.PlayedFightSnd = FALSE;
		pAvatar->attrib.IsJumpEnhanced = FALSE;
		pAvatar->attrib.DiedLastFrame = FALSE;
		pAvatar->attrib.BeingControlled = FALSE;
		pAvatar->Version 	= NULL;
		pAvatar->Id 		= NULL;
		pAvatar->Status 	= CAvatar::AI_INIT;
		pAvatar->hPlayerStats = fERROR;
		pAvatar->Realm.HomeRealm	= REALM::NO_COUNTRY;
		pAvatar->Engaged	= 0;
		pAvatar->hEnemy	= fERROR;
		pAvatar->KillType	= NO_THING;
		pAvatar->fDamage = -1;
		pAvatar->fAttackTurnFlag = ATTACK_TURN_NOT_DECIDED;
		pAvatar->fDamageFlag = 0;
		pAvatar->fOriginalRemapColor = 0;
		pAvatar->fHighlightEndTime = ULONG_MAX;
		pAvatar->fContactDistance = 0;
		pAvatar->UnitIndex = -1;
		
		for (i = 0; i < MAX_ATTACK_ROLLS; ++i)
			pAvatar->fAttackRolls[i] = 0;
			
		pAvatar->fFightSeq.mfInit();
		
		pAvatar->CurSequence= INITSEQ;
		pAvatar->SetAIFuncIndex(AI_FUNC_NULL);
		
		pAvatar->fPrev.Sequence	= INITSEQ;
		pAvatar->fPrev.A		= 0;
	 	pAvatar->fPrev.Status	= pAvatar->Status;
	 	
		// Initialize the snd array.
		memset(&(pAvatar->SndData[0].SndId), 0, MAX_SND_QUEUE*sizeof(AVATAR_SND_DATA));
	}

	return hAvatar;   // return handle	
}

/* ========================================================================
   Function - mfCreateFireBall
   Description - create a fireball
   Resturns - void
   ======================================================================== */
void CAvatar::mfCreateFireBall(
			   LONG Id,
			   THINGTYPE Type,
			   LONG DamageLevel) const
{
	
	PTR_SCENE	pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	POINT		PtInFront;
	LONG		Angle;
	
	PtInFront.x = 0;
	PtInFront.y = 2 * mfWidth();
	Rotate(&PtInFront, mfAngle());
	PtInFront.x += mfX();
	PtInFront.y += mfY();
	
	if(hEnemy != fERROR)
	{
		CAvatar const * const pEnemy = (CAvatar const * const)BLKPTR(hEnemy);
		Angle = AngleFromPoint(PtInFront.x,
							  			  PtInFront.y,
							   			  pEnemy->mfX(), pEnemy->mfY(), RESOLUTION_4);
	}
	else
	{
		CAvatar const * const pAvatar = (CAvatar const * const)BLKPTR(hThis);
		Angle = pAvatar->mfAngle();
	}
	
	LONG const i=CAvatar::CreateAvatar(Id, Type,
							PtInFront.x,
						    PtInFront.y,
						    Angle,
						  AI_FUNC_FIREBALL, pCurrentScene,
						  Realm.HomeRealm);
	if ( i >= 0)
	{
		CAvatar * pFireball = (CAvatar *) BLKPTR (pCurrentScene->Avatars[i]);
		pFireball->mfMoveToXYZA(pFireball->mfX(), pFireball->mfY(), mfZ()+mfHeight()-33, pFireball->mfAngle());
		pFireball->fDamage = DamageLevel;
		pFireball->ThingType = Type;
		pFireball->hEnemy=hEnemy;	
		pFireball->fFireBall.hWhoCast = hThis;
		pFireball->fDamageFlag = 1;
	}
#if defined (_DEBUG)
	else
	{
		fatal_error("CreateFireBall ERROR! Unable to create fireball.\n");
	}
#endif
}

/* ========================================================================
   Function - mfCreateFireBall
   Description - create a fireball
   Resturns - void
   ======================================================================== */
void CAvatar::mfCreateHorse(
			   LONG Id,
			   THINGTYPE Type ) const
{
	
	PTR_SCENE	pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	LONG		Angle;
	
	CAvatar const * const pAvatar = (CAvatar const * const)BLKPTR(hThis);
	Angle = 32;
	
	LONG const i=CAvatar::CreateAvatar( Id, Type,
										pAvatar->mfX(),
										pAvatar->mfY(),
										Angle,
										AI_FUNC_HORSE,
										pCurrentScene,
						  				REALM::NO_COUNTRY);
	if ( i >= 0)
	{
		CAvatar * pHorse = (CAvatar *) BLKPTR (pCurrentScene->Avatars[i]);
		pHorse->ThingType = Type;
		pHorse->hEnemy=fERROR;	
	}
#if defined (_DEBUG)
	else
	{
		fatal_error("CreateHorse ERROR! Unable to create Horse.\n");
	}
#endif
}

/* ========================================================================
   Function    - mfInflictMagicDamage
   Description - checks to see if a kill has occured then checks resistances
   Returns     - TRUE for fatal, FALSE for alive
   ======================================================================== */
BOOL CAvatar::mfInflictMagicDamage(
	CAvatar * const pEnemy,
	THINGTYPE const NewKillType,
	LONG const Damage)
{
	SHORT	result = 0;
		
	if(NewKillType != NO_THING &&
	   !mfAmIImmoblized() &&
	   pEnemy->Status != AI_DEAD)
	{
		if (pEnemy->AIFuncIndex == AI_FUNC_BATTLE_CAPTAIN)
		{
			pEnemy->KillType = NewKillType;
			pEnemy->fDamage = Damage;
		}
		else
		{
			
			SET_NOT_FRIENDLY(Realm.HomeRealm, pEnemy->Realm.HomeRealm);
			if (pEnemy->hPlayerStats == fERROR ||
			    hPlayerStats == fERROR)
			{
				pEnemy->Status = AI_DEAD;
				pEnemy->mfPlayAnimationOnce(EXPIRESEQ);
				result = Damage;
			}
			else
			{
				DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hPlayerStats);
				DumbAutoLockPtr<PLAYER_STATS> const pEnemyStats(pEnemy->hPlayerStats);
				PLAYER_HIT PlayerHitObj(pPlayerStats, pEnemyStats);
				
				result = fDamage = PlayerHitObj.mfInflictMagicDamageOnTarget( NewKillType,
											                                   Damage);
				// check to see if this kills the enemy
				if(pEnemyStats->mfGetCurHitPoints() <= 0)
				{
					if (pEnemy->Status == AI_STONE)
					{
						mfKillAvatar(pEnemy);
						pEnemy->mfPlayAnimationOnce(ATTACK1SEQ);
						pEnemy->mfSetType(CRUMBLE_1);
					}
					else
					{
						mfKillAvatar(pEnemy);
						// [d11-27-96 JPC] Removed MAGIC_MISSILE_1 test per Gary's request.
					    if (NewKillType == FIREBALL_1)
					    {
							if (GAME_TTYPE::mfIsHuman(pEnemy->mfType()))
							{
								pEnemy->mfPlayAnimationOnce(ATTACK1SEQ);
							}
							else
							{
								pEnemy->mfPlayAnimationOnce(EXPIRESEQ);
							}
							pEnemy->mfSetType( NewKillType);
							pEnemy->SetAIFuncIndex( CAvatar::AI_FUNC_FIREBALL );
						}
						else
						if (NewKillType == CRUMBLE_1 ||
						    NewKillType == VAPORIZE_1 )
						{
							pEnemy->mfPlayAnimationOnce(ATTACK1SEQ);
							pEnemy->mfSetType( NewKillType);
						}
					}
				}
				else
				if (result > 3)
				{
					pEnemy->mfPainSounds();
				}
			}
		}
	}
	fDamageFlag = 0;
		
	return result;	
}
/* ========================================================================
   Function	- DeleteAvatar
   Description - Delete an avatar from the list and release its memory.
   Returns	 - void
   ======================================================================== */

void CAvatar::DeleteAvatar(SHORT hAvatar)
{
	CAvatar *pAvatar = (CAvatar *) BLKPTR(hAvatar);
		
	PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	LONG i;
	for(i=0; i<MAX_AVATARS&&pCurrentScene->Avatars[i]!=pAvatar->hThis; ++i)
	{}
	
	pCurrentScene->Avatars[i]=fERROR;
	if (pAvatar->UnitIndex != -1)
		units[pAvatar->UnitIndex].iHandle = fERROR;
	
	// give the AI a chance to throw it's stuff away
	switch(pAvatar->GetAIFuncIndex())
	{
		case AI_FUNC_FOLLOW_PATH:
			pAvatar->fFollowPath.mfDelete();
			break;
		case AI_FUNC_FOLLOW_PLAYER:
			pAvatar->fFollowPlayer.mfDelete();
		    break;
		case AI_FUNC_BATTLE_CAPTAIN:
	    	pAvatar->fBtlCap.mfDelete();
	    	break;
	}
	
	// disable this in the things array
	purge_thing(pAvatar->ThingIndex);
	remove_thing(pAvatar->ThingIndex);
	
	// get rid of his stats
	if (pAvatar->hPlayerStats != fERROR)
	{
		DumbHandlePtr<PLAYER_STATS> const pPlayerStats(pAvatar->hPlayerStats);

		pPlayerStats->mfDelete();
		if (pAvatar->attrib.RuntimeCreated)
		{
			DisposBlock(pAvatar->hPlayerStats);
		}
		
		pAvatar->hPlayerStats = fERROR;	// just incase something else looks here.
	}
	
	// get rid of the memory
	DisposBlock(hAvatar);
}


/* ========================================================================
   Function	- CreateAvatar
   Description - Create an avatar of a given type at given location
   Returns	 - index in Avatars array or fERROR if unable to create.
   ======================================================================== */

LONG CAvatar::CreateAvatar(
	LONG	Id,
	THINGTYPE	Type,
	LONG	X,
	LONG	Y,
	LONG	A,
	CAvatar::AIFUNC	FuncIndex,
	PTR_SCENE	pScene,
	const REALM::REALM_TYPE	Realm
)
{
	LONG	i;
	
	
	// find an empty Avatar slot, assume for now that we never run out
	// if find a NULL pointer, allocate space for AVATAR structure
	for (i=0; i<MAX_AVATARS && pScene->Avatars[i] != fERROR; ++i)
	{}
	
	if (i >= MAX_AVATARS)
	{
		i = fERROR; 			// We have no more slots!
		printf("WARNING! Out of Avatar slots!\n");
	}
	else
	{
		LONG lMemoryAvail = ReportFreeMem(TRUE);
		printf("Memory available before creating avatar %ld\n", lMemoryAvail);
		// create the avatar
		pScene->Avatars[i] = CAvatar::NewAvatar();
	
		if (pScene->Avatars[i] != fERROR)
		{
			printf("Creating Avatar Id = %ld\n", Id);
			
			//GEH SetBlockAttr(pScene->Avatars[i],LOCKED, LOCKED);
			CAvatar *pAvatar = (CAvatar *) BLKPTR(pScene->Avatars[i]);
		
			pAvatar->Id = Id;
			pAvatar->UnitIndex = -1;
			pAvatar->Status = CAvatar::AI_INIT;
			pAvatar->mfDefineCThing(Type, X, Y, point_to_floor_height(X,Y), A, 0, 0, 0);
			pAvatar->SetAIFuncIndex(FuncIndex);
			pAvatar->mfInitVals();
			pAvatar->Realm.HomeRealm = Realm;
			pAvatar->mfSetOriginalRemapColor(Realm);
			printf("Used memory = %ld\n", (lMemoryAvail - ReportFreeMem(TRUE)));
			
			//GEH ClrLock(pScene->Avatars[i]);
		}
		else
		{
			printf("WARNING! Unable to create Avatar Id = %ld\n", Id);
			i = fERROR;
		}
	}
	
	return( i );
}
	
/* ========================================================================
   Function	- mfRotateToward
   Description - Given an angle generate a new angle that is closer to it.
				 Call this fn until you are at your requested angle.
   Returns	 - New angle
   ======================================================================== */

LONG CAvatar::mfRotateToward(LONG CurrentAngle,
							 LONG TargetAngle)
	
{
	LONG da;
	LONG lNewAngle;
	
	da = CurrentAngle - TargetAngle;
	if (da)
	{
		const LONG TurnRate = 15;
		
		if (da < 0)
			da += 256;
		
		if (da >= 128)
		{
			lNewAngle = (CurrentAngle + MIN(abs(da),TurnRate)) % 256;
		}
		else
		{
			lNewAngle = CurrentAngle - MIN(abs(da),TurnRate);
			if (lNewAngle < 0)
				lNewAngle += 256;
		}
	}
	else
	{
		lNewAngle = TargetAngle;
	}
	mfSetAngle(lNewAngle);
	
	return lNewAngle;
}


#if 0 // UNUSED
/* ========================================================================
   Function	- mfSwitchToGuardPt
   Description - send the avatar on the way.
   Returns	 - void
   ======================================================================== */
void CAvatar::mfSwitchToGuardPt(LONG hAvatar)
{
	CAvatar *pAvatar = (CAvatar *) BLKPTR((SHORT) hAvatar);
	
	pAvatar->SetAIFuncIndex(AI_FUNC_GUARD_POINT);
	pAvatar->fGuardPt.X = pAvatar->mfX();
	pAvatar->fGuardPt.Y = pAvatar->mfY();
	
	// GWP For now just have them stand a bit to the side,
	// GWP and hope its not in the Wall.
	// GWP Later put up the map and get a point off it.
	pAvatar->fGuardPt.StandAsideX = pAvatar->mfX() + (2 * pAvatar->mfGetWalkRate());
	pAvatar->fGuardPt.StandAsideY = pAvatar->mfY() + (2 * pAvatar->mfGetWalkRate());
	
	pAvatar->Status = AI_END_LISTEN;
}
#endif // UNUSED

#if 0	// UNUSED
/* ========================================================================
   Function	- mfSwitchToStandStill
   Description - send the avatar on the way.
   Returns	 - void
   ======================================================================== */
void CAvatar::mfSwitchToStandStill(LONG hAvatar)
{
	CAvatar *pAvatar = (CAvatar *) BLKPTR((SHORT) hAvatar);
	
	pAvatar->SetAIFuncIndex(AI_FUNC_STAND_STILL);
	pAvatar->Status = AI_END_LISTEN;
}
#endif	// UNUSED

/* ========================================================================
   Function	   - mfCheckSnds
   Description - check to see if this sequence/frame fires a sound
   Returns	   - void
   ======================================================================== */
void CAvatar::mfCheckSnds()
{
	for (LONG i = 0; i < MAX_SND_QUEUE; ++i)
	{
		// if the Trigger is valid, check if we have passed it
		if(SCENE_MGR::gFrame > SndData[i].TriggerFrame)
		{
			// Play the sound.
			if(SndData[i].SndId != 0)
			{
				AddSndObj(
					SndData[i].SndId,
					SndData[i].Range,
					ThingIndex);
			}
			// clear the tick
			if(SndData[i].Loop == FALSE)
				SndData[i].TriggerFrame = MAX_TICK;
			else
				SndData[i].TriggerFrame += SndData[i].TotalFrames;
		}
	}
}
/* ========================================================================
   Function    - mfQueueSnd
   Description - Queue up a sound to be played at some future frame cnt.
   				 If the queue is full, it won't be added.
   Returns     -
   ======================================================================== */
void CAvatar::mfQueueSnd(BIRTHRT_SND const SndId,
	               SHORT const range,
	               LONG const inNumbFrames)
{
	for (SHORT i = 0; i < MAX_SND_QUEUE; ++i)
	{
		if (SndData[i].SndId == SndId && SndData[i].TriggerFrame != MAX_TICK)
			return;							 // HACK, HACK, HACK!!!!!!!!
	}
	for (SHORT i = 0; i < MAX_SND_QUEUE; ++i)
	{
		if (SndData[i].TriggerFrame == MAX_TICK)
		{
			SndData[i].TriggerFrame = SCENE_MGR::gFrame + inNumbFrames;
			SndData[i].SndId = SndId;
			SndData[i].Range = range;
			SndData[i].Loop = FALSE;
			break;
		}
	}
}

/* ========================================================================
	Function   	-mfSetLoopingSnd
	Description -loop sound, the sound has to be in the queue already
	Returns     -
	======================================================================= */
void CAvatar::mfSetLoopingSnd(BIRTHRT_SND const SndId, SBYTE totalFrames)
{
	for(LONG  i = 0; i< MAX_SND_QUEUE; ++i)
	{
		if(SndData[i].SndId == SndId )
		{
			SndData[i].Loop = TRUE;
			SndData[i].TotalFrames = totalFrames;
		}
	}
}

/* ========================================================================
	Function     -	mfClearLoopingSnd
	Description  - 	stop the looping sound
	Returns      -
   ========================================================================*/
void CAvatar::mfClearLoopingSnd(BIRTHRT_SND const SndId)
{
	for(LONG i=0; i<MAX_SND_QUEUE; ++i)
	{
		if(SndData[i].SndId == SndId)
		{
			SndData[i].Loop = FALSE;
			SndData[i].TriggerFrame = MAX_TICK;
			break;
		}
	}
}

/* ========================================================================
	Function     -	mfClearAllLoopingSnds
	Description  - 	stop All of the looping sounds
	Returns      -
   ========================================================================*/
void CAvatar::mfClearAllLoopingSnds()
{
	for(LONG i=0; i<MAX_SND_QUEUE; ++i)
	{
		if(SndData[i].Loop == TRUE)
		{
			SndData[i].Loop = FALSE;
			SndData[i].TriggerFrame = MAX_TICK;
			break;
		}
	}
}

/* ========================================================================
   Function    - AvatarIdToHdl
   Description - find the Id, return the hThis
   				 Note: If you can store the handle not the ID. Unless the
   				       game could kill off the avatar and not notify you.
   Returns     -
   ======================================================================== */
SHORT AvatarIdToHdl ( LONG Id )
{
	SHORT	i;
	SHORT	hdl = (SHORT)fERROR; // assume id not found
	PTR_SCENE pCurrentScene = 0;
	
	pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	for(i=0;i<MAX_AVATARS;++i)
	{
		if (pCurrentScene->Avatars[i] != fERROR)
		{
			CAvatar const * const pA = (CAvatar const * const) BLKPTR(pCurrentScene->Avatars[i]);
			if(pA->Id == Id)
			{
				hdl = pA->hThis;
				break;
			}
		}
	}
	
	return hdl;
}

/* ========================================================================
   Function    - FindClosestFoe
   Description - Search the entire Avatar list and find the closest unengaged
                 foe. Of the 5 closest foes, pick the closest one with the
                 least number of engagements.
                 Note: This may find that the nearest foe is the one behind
                 a wall, and thus we may want to check the visibilty flag.
   Returns     - handle to that avatar.
   ======================================================================== */
#define MAX_CLOSEST	5

// GWP The linked list stuff is broken...fix later.
// GWP until then use the simplified version which follows.
#if 0
HDL_AVATAR CAvatar::mfFindClosestFoe() const
{
	PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	SHORT i;
	SHORT HeadOfList = 0;
	SHORT TailOfList = MAX_CLOSEST - 1;
	SHORT Result = fERROR;
	SHORT minimumEngaged;
	struct _Closest {
		LONG distance;
		SHORT hAvatar;
		SHORT Engaged;
		SHORT next;
		SHORT prev;
	} Closest[MAX_CLOSEST];
	
	// Initialize the closest group array.
	for (i = 0; i < MAX_CLOSEST; ++i)
	{
		Closest[i].distance = LONG_MAX;
		Closest[i].hAvatar = fERROR;
		Closest[i].Engaged = 0;
		Closest[i].next = (SHORT)(i + 1);
		Closest[i].prev = (SHORT)(i - 1);
	}
	// mark the ends of the list.
	Closest[0].prev = fERROR;
	Closest[MAX_CLOSEST -1].next = fERROR;
	
	for (i = 0; i < MAX_AVATARS; ++i)
	{
		if (pCurrentScene->Avatars[i] != fERROR)
		{
			CAvatar * pAvatar = (CAvatar *) BLKPTR(pCurrentScene->Avatars[i]);
			if (hThis != pCurrentScene->Avatars[i] &&
			    !pAvatar->mfAmIImmoblized()
			    !mfIsAlly(pAvatar))
			{
				SHORT index;
				const LONG distance = aprox_dist(mfX(), mfY(),
				                        pAvatar->mfX(), pAvatar->mfY());
				
				// See if we are smaller than any other distance.
				for (index = HeadOfList;
				    index != fERROR && Closest[index].distance < distance;
					index = Closest[index].next)
				{ }
				
				// index is now either fERROR or the location at which to insert
				// the new distance and avatar handle.
				
				// Add this hAvatar to the list.
				if (index != fERROR)
				{
					SHORT j;
					SHORT AvailableSlot = 0;
					
					// Look for the first unused slot in the array
					for (j = HeadOfList;
					     Closest[j].hAvatar != fERROR;
					     j = Closest[j].next)
					{
					}
					
					if (j == fERROR)
					{
						// No unused slots use the last one in the list.
						AvailableSlot = TailOfList;
						// Now move the tail back up one in the list.
						// Cause we just reused the old tail location.
						TailOfList = Closest[TailOfList].prev;
					}
					else
					{
						AvailableSlot = j;
					}
					
					// insert the data into the list in sorted order.
					
					// Connect the pointers.
					if (Closest[index].prev != fERROR)
					{
						
						Closest[Closest[index].prev].next = AvailableSlot;
						
						if (Closest[AvailableSlot].next != fERROR)
						{
							Closest[Closest[AvailableSlot].next].prev = Closest[AvailableSlot].prev;
						}
						
						const SHORT oldAvailableNext = Closest[AvailableSlot].next;
						
						Closest[AvailableSlot].next = index;
						Closest[AvailableSlot].prev = Closest[index].prev;
						
						Closest[index].prev = AvailableSlot;
						Closest[index].next = oldAvailableNext;
					}
					else
					{
						// We must be pointing at the head of the list.
						HeadOfList = AvailableSlot;
						
						const SHORT oldAvailableNext = Closest[AvailableSlot].next;
						
						if (Closest[AvailableSlot].next != fERROR)
						{
							Closest[Closest[AvailableSlot].next].prev = Closest[AvailableSlot].prev;
						}
						
						Closest[AvailableSlot].next = index;
						Closest[AvailableSlot].prev = fERROR;
						
						Closest[index].next = oldAvailableNext;
						Closest[index].prev = AvailableSlot;
						
					}
					
					
					Closest[AvailableSlot].distance = distance;
					Closest[AvailableSlot].hAvatar = pCurrentScene->Avatars[i];
					Closest[AvailableSlot].Engaged = (SHORT)(pAvatar->attrib.Engaged);
				}
			}
		}
	}
	
	// Now pick the one with the fewest Engaged attackers.
	minimumEngaged = Closest[HeadOfList].Engaged;
	Result = Closest[HeadOfList].hAvatar;
	for (i = 0; i< MAX_CLOSEST; ++i)
	{
		if (Closest[i].hAvatar != fERROR &&
		    minimumEngaged < Closest[i].Engaged)
		{
			Result = Closest[HeadOfList].hAvatar;
		}
	}
	return Result;
}
#else
// just find the closest enemy.
HDL_AVATAR CAvatar::mfFindClosestFoe() const
{
	PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	SHORT i;
	HDL_AVATAR Result = fERROR;
	LONG prevDistance = LONG_MAX;
	
	
	for (i = 0; i < MAX_AVATARS; ++i)
	{
		if (pCurrentScene->Avatars[i] != fERROR)
		{
			CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(pCurrentScene->Avatars[i]);
			
			// Check that its first not me.
			// And not Immoblized
			// And not my ally
			
			if (hThis != pCurrentScene->Avatars[i] &&
				!pAvatar->mfAmIImmoblized() &&
				!mfIsAlly(pAvatar) &&
				pAvatar->AIFuncIndex != AI_FUNC_FIREBALL
				)
			{
				// if I can see his sector, check to see if he's closest
				if(reject( mfGetSector(), pAvatar->mfGetSector()))
				{
					LONG const distance = aprox_dist(mfX(), mfY(),
											pAvatar->mfX(), pAvatar->mfY());
					
					// See if we are smaller than the previous distance.
					if (distance < prevDistance)
					{
						Result = pCurrentScene->Avatars[i];
						prevDistance = distance;
					}
				}
			}
		}
	}
	return Result;
}
/* ========================================================================
   Function	- FindClosestVisibleFoe
   Description - The foe's must be visible to you to "count."
   Returns	 - Handle to the foe.
   ======================================================================== */
// just find the closest enemy.
HDL_AVATAR CAvatar::mfFindClosestVisibleFoe() const
{
	PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	SHORT i;
	HDL_AVATAR Result = fERROR;
	LONG prevDistance = LONG_MAX;
	HDL_AVATAR HackOnMe = fERROR;
	LONG prevHackOnMeDistance = LONG_MAX;
	
	
	for (i = 0; i < MAX_AVATARS; ++i)
	{
		if (pCurrentScene->Avatars[i] != fERROR)
		{
			CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(pCurrentScene->Avatars[i]);
			
			// Check that its first not me.
			// And not Immoblized
			// And not my ally
			
			if (hThis != pCurrentScene->Avatars[i] &&
				!pAvatar->mfAmIImmoblized() &&
				!mfIsAlly(pAvatar) &&
				pAvatar->AIFuncIndex != AI_FUNC_FIREBALL
				)
			{
				// if I can see his sector, check to see if he's closest
				if(reject( mfGetSector(), pAvatar->mfGetSector()))
				{
					LONG const distance = aprox_dist(mfX(), mfY(),
											pAvatar->mfX(), pAvatar->mfY());
					
					if (pAvatar->hEnemy == hThis && Engaged > 0)
					{
						if (distance < prevHackOnMeDistance)
						{
							HackOnMe = pCurrentScene->Avatars[i];
							prevHackOnMeDistance = distance;
						}
					}
					
					// See if we are smaller than the previous distance.
					if (distance < prevDistance)
					{
						PLAYER	TempPlayer;
						LONG Angle;
						LONG BumpDistance;
						
						Angle = AngleFromPoint2 ( mfX(), mfY(),
												  pAvatar->mfX(),
												  pAvatar->mfY(), RESOLUTION_1);
						TempPlayer.x = mfX() << PLAYER_FIXEDPT;
						TempPlayer.y = mfY() << PLAYER_FIXEDPT;
						TempPlayer.z = mfZ() + 32;
						TempPlayer.p = 0;
						TempPlayer.a = mfAngle();
						TempPlayer.Flying = TRUE;
						TempPlayer.w = 16;
						TempPlayer.h = 16;
						TempPlayer.bump = iNOTHING;
						TempPlayer.ThingIndex = fERROR;
						
						CheckLongMove(&TempPlayer,
						          Angle,
								  distance,
						          0,
						          pAvatar->mfZ(),
						          &BumpDistance);
						
						switch(TempPlayer.bump)
						{
						case iNOTHING:
						case iSHALLOW_WATER:
						case iDEEP_WATER:
						case iLAVA:
						case iACID:
						
							Result = pCurrentScene->Avatars[i];
							prevDistance = distance;
							break;
						case iFLOOR:
						    if (GAME_TTYPE::mfCanFly(mfType()))
						    {
								Result = pCurrentScene->Avatars[i];
								prevDistance = distance;
							}
							break;
						}
					}
				}
			}
		}
	}
	
	// If I for some reason can't see the avatar wacking on me,
	// wack back at the one who is hitting me. -GWP-
	if (Result == fERROR)
	{
		Result = HackOnMe;
	}
	return Result;
}

/* ========================================================================
   Function	- MoveTowardWithBump
   Description - Move to withing BUMP distance from a point by my Rate amount
   Returns	 - distance from point
   ======================================================================== */

WadThingType CAvatar::mfMoveTowardWithBump(
		LONG	distance,
		LONG	TargetX,
		LONG	TargetY,
		LONG	Bump,
		LONG	Rate,
		SHORT	sSpecial,
		LONG	*pAngle,
		LONG    *pmyThingBumped
)
{
	PLAYER TempPlayer;
	LONG BumpDistance;
	WadThingType bumpResult;	// [d11-30-96 JPC] we need to manipulate this for flying things
	LONG bumpObjectType;

	if(abs(distance) > Bump)
	{
		FIXED_VECTOR_3D NewVector;
		LONG dx = TargetX - mfX();
		LONG dy = TargetY - mfY();
			
		if (distance > 0)
			distance -= Bump;
		else
			distance += Bump;
		
		
		if (Bump > 2)
		{
			// Introduce some randomness as to exactly where
			// we endup within "Bump" distance of the target.
			// Determine whether we are to the N/S of the target or E/W
			// So we woble on the short axis.
			if (ABS(dx) > ABS(dy))
			{
				if (random(2))
				{
					dy += random(Bump - 1);
				}
				else
				{
					dy -= random(Bump - 1);
				}
			}
			else
			{
				if (random(2))
				{
					dx += random(Bump - 1);
				}
				else
				{
					dx -= random(Bump - 1);
				}
			}
		}
		
		FaceTo(TargetX, TargetY);
		mfConvertPositionToPlayer(TempPlayer);
		
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
		
		
		// For now ignore the noise we make if we hit something.
		// GWP optimize by not doing this check if we're not "visible."
		// GWP if we can't be seen bumping objects who cares?
		//if (mfIsVisible())
		{
			bumpObjectType = CheckBump(&TempPlayer, &NewVector, Rate);
		}
		
		bumpResult = CheckMove(&TempPlayer, &NewVector, sSpecial, pAngle, &BumpDistance);

		// [d11-30-96 JPC] For a flying thing, treat various nasty sectors as nothing.
		if (attrib.IsFlying)
		{
			if (bumpResult == iLAVA ||
			    bumpResult == iSHALLOW_WATER ||
			    bumpResult == iDEEP_WATER ||
			    bumpResult == iHOLE ||
			    bumpResult == iACID)
				bumpResult = iNOTHING;
		}
		
		if (bumpResult == iNOTHING &&
		    bumpObjectType != 0)
		{
			bumpResult = iOBJECT;
			TempPlayer.bump = iOBJECT;
		}

		switch (bumpResult)
		{
		case iOBJECT:
		//case iSLIDE_ON_WALL:		// GWP Seems broken.
		case iSHALLOW_WATER:
		case iNOTHING:
			// [d11-26-96 JPC] Before doing the move, make SURE we are not
			// moving into a nasty sector.
			if (attrib.IsFlying || !mfIsNastySector (mfX() + PLAYER_INT_VAL(NewVector.dx),
				mfY() + PLAYER_INT_VAL(NewVector.dy)))
			{
				mfMoveToXYA(mfX() + PLAYER_INT_VAL(NewVector.dx),
					 mfY() + PLAYER_INT_VAL(NewVector.dy),
					 mfAngle());
			}
			break;
		}
   }
	else
	{
		FaceTo(TargetX, TargetY);
	}
	
	
	*pmyThingBumped = TempPlayer.BumpIndex;
	return (TempPlayer.bump);
}

#endif

/* ========================================================================
   Function	- WackEm
   Description - Hit your enemy with real damage points.
   Returns	 - TRUE (scored a hit, FALSE missed.)
   ======================================================================== */

BOOL CAvatar::mfWackEm(const HDL_AVATAR hEnemyTarget)
{
	LONG HitValue = 0;
	BOOL bHitOrNot = 0;
	LONG i;
	LONG j;
	
	fDamage = 0;
	
	if (hEnemyTarget == fERROR)
	{
		fDamageFlag = 0;
		return FALSE;
	}
	
	//GEH SetBlockAttr(hEnemyTarget, LOCKED, LOCKED);
	CAvatar * const pEnemyTarget = (CAvatar * const) BLKPTR(hEnemyTarget);
	
	if (pEnemyTarget->mfAmIImmoblized())
	{
		// Hey you're hitting a dead thing.
		mfSethEnemy(fERROR);
		fDamageFlag = 0;
		return FALSE;
	}
	
	SET_NOT_FRIENDLY(Realm.HomeRealm, pEnemyTarget->Realm.HomeRealm);
	if (pEnemyTarget->hPlayerStats != fERROR && hPlayerStats != fERROR)
	{
		//GEH SetBlockAttr(hPlayerStats, LOCKED, LOCKED);
		//GEH SetBlockAttr(pEnemyTarget->hPlayerStats, LOCKED, LOCKED);
		DumbAutoLockPtr<PLAYER_STATS> const pAvatarStats(hPlayerStats);
		DumbAutoLockPtr<PLAYER_STATS> const pEnemyStats(pEnemyTarget->hPlayerStats);
		
		LONG const Modifier = mfCalculateAttackModifier(hEnemyTarget);
		
		PLAYER_HIT PlayerHitObj(pAvatarStats, pEnemyStats);
		
		for (i = 0, j = 0; i < fDamageFlag; ++i)
		{
			LONG const HitThisTime = PlayerHitObj.mfRollAttackTarget( Modifier,
																&fAttackRolls[j]);
			if (HitThisTime == TRUE)
			{
				bHitOrNot = TRUE;
				HitValue += PlayerHitObj.mfInflictDamageOnTarget(pEnemyTarget->mfType());
			}
			
			// Its possible we won't save all the hit rolls for every type of
			// avatar. Most regents only have 2 attacks at best.
			if ( j < MAX_ATTACK_ROLLS)
				++j;
		}
		
		// Make an attack noise.
		
		if (bHitOrNot == TRUE)
		{
			if (fDamageFlag > 0)
			{
				fDamage = HitValue;
			}
		
			mfFightHitSounds(pAvatarStats, pEnemyTarget, pEnemyStats);
		
	   		// Check for severe injury.
	   		if (pEnemyStats->mfGetCurHitPoints() > 0)
	   		{
		   		if (HitValue > 3 )
		   		{
					if (WEAPON_INFO::mfGetStyle(pAvatarStats->mfGetFirstWeapon()) == WEAPON_INFO::BLUDGEONING)
					{
						pEnemyTarget->mfQueueSnd(SND_BONE_CRUSH1, SND_BONE_CRUSH_TOTAL, BIG_HIT_FRAME_CNT);
			   		}
			   		
			   	    pEnemyTarget->mfPainSounds();
				}
			}
			else
			{
				mfKillAvatar(pEnemyTarget);
			}
		}
		//GEH ClrLock(pEnemyTarget->hPlayerStats);
		//GEH ClrLock(hPlayerStats);
	}
	else
	{
		mfKillAvatar(pEnemyTarget);
	}
	
	fDamageFlag = 0;
		
	//GEH ClrLock(hEnemyTarget);
	
	return TRUE;
}
/* ========================================================================
   Function    - mfPainSounds
   Description - Set up a pain sound for the fight sequences.
   Returns     -
   ======================================================================== */

void CAvatar::mfPainSounds()
{

	switch(GAME_TTYPE::mfGetSoundType(mfType()))
	{
	case HARPY_SOUNDS:
		mfQueueSnd(SND_FEMALE_SCREAM1, SND_FEMALE_SCREAM_TOTAL, PAIN_FRAME_CNT);
		break;
	case DOG_SOUNDS:
		switch(random(3))
		{
		case 0:
	   		mfQueueSnd(SND_DOG_WHIMPER1, SND_DOG_WHIMPER_TOTAL, PAIN_FRAME_CNT);
	   		break;
		case 1:
	   		mfQueueSnd(SND_DOG_YELP1, SND_DOG_YELP_TOTAL, PAIN_FRAME_CNT);
	   		break;
		case 2:
	   		mfQueueSnd(SND_DOG_HOWL1, SND_DOG_HOWL_TOTAL, PAIN_FRAME_CNT);
	   		break;
		}
		break;
	case FEMALE_SERVANT_SOUNDS:
	case FEMALE_SOLDIER_SOUNDS:
	case FEMALE_LORD_SOUNDS:
		mfQueueSnd(SND_FEMALE_GRUNT1, SND_FEMALE_GRUNT_TOTAL, PAIN_FRAME_CNT);
		break;
	case SPIDER_KING_SOUNDS:
	case RHUOBHE_SOUNDS:
	case ZOMBIE_SOUNDS:
	case GOBLIN_SOUNDS:
	case MALE_SERVANT_SOUNDS:
	case MALE_SOLDIER_SOUNDS:
	case MALE_LORD_SOUNDS:
		mfQueueSnd(SND_MALE_GRUNT1, SND_MALE_GRUNT_TOTAL, PAIN_FRAME_CNT);
		break;
	case OGRE_SOUNDS:
		mfQueueSnd(SND_OGRE_PAIN1, SND_OGRE_PAIN_TOTAL, PAIN_FRAME_CNT);
		break;
	case GNOLL_SOUNDS:
		mfQueueSnd(SND_GNOLL_PAIN1, SND_GNOLL_PAIN_TOTAL, PAIN_FRAME_CNT);
		break;
	case OTYUGH_SOUNDS:
		mfQueueSnd(SND_OTYUGH_PAIN1, SND_OTYUGH_PAIN_TOTAL, PAIN_FRAME_CNT);
		break;
	case GORGON_SOUNDS:
		mfQueueSnd(SND_GORGON_PAIN1, SND_GORGON_PAIN_TOTAL, PAIN_FRAME_CNT);
		break;
	case WRAITH_SOUNDS:
	case SPECTRE_SOUNDS:
		mfQueueSnd(SND_WRAITH_PAIN1, SND_WRAITH_PAIN_TOTAL, PAIN_FRAME_CNT);
		break;
	case ANKHEG_SOUNDS:
		mfQueueSnd(SND_ANKHEG_PAIN1, SND_ANKHEG_PAIN_TOTAL, PAIN_FRAME_CNT);
		break;
	default:		// No pain sound for this type.
		break;
	}
}
/* ========================================================================
   Function    - mfKillAvatar
   Description - kill this guy, dead
   Returns     - void
   ======================================================================== */
void CAvatar::mfKillAvatar(CAvatar * const pEnemyAvatar)
{
	pEnemyAvatar->fDamageFlag = 0;
	
	if(pEnemyAvatar->hPlayerStats == fERROR)
	{
		pEnemyAvatar->mfPlayAnimationOnce(EXPIRESEQ);
		pEnemyAvatar->Status = AI_DEAD;
		return;
	}
		
	DumbAutoLockPtr<PLAYER_STATS> const pEnemyAvatarStats(pEnemyAvatar->hPlayerStats);
	
	if (hPlayerStats == fERROR)
	{
		return;
	}
	
	DumbAutoLockPtr<PLAYER_STATS> const pMyPlayerStats(hPlayerStats);
	

	// Deactivate all enemy's items
	ItemEffects::DeactivateAvatarsItems(pEnemyAvatar->hPlayerStats);



	// Then drop them
//	pEnemyAvatar->mfSpewAll();
	//They now get dropped in their AI function....

	
	// You've killed'em.
	pEnemyAvatarStats->mfKillem();
	if(ThingType == SPECTRE || ThingType == T_SPECTRE
	   ||ThingType == WRAITH || ThingType == T_WRAITH)
	   pEnemyAvatar->attrib.KilledBySpectre = TRUE;

	// [d11-05-96 JPC] Add dead thing to adjacent subsectors to help with
	// clipping problem.
#if EXTRATHINGS
	//AddThingToAdjacentSubsectors (pEnemyAvatar->ThingIndex);
#endif

	// If the Enemy had a target DisEngage that one.
	if (pEnemyAvatar->hEnemy != fERROR)
	{
		pEnemyAvatar->mfSethEnemy(fERROR);
	}
	mfSethEnemy(fERROR);
	
	// Hellhounds breath fire for attack1
	if ((mfType() == T_HELL_HOUND ||
	     mfType() == HELL_HOUND) &&
		(attrib.AttackMode == FIGHT_SEQUENCE::ATM_QUICK_HIGH ||
		 attrib.AttackMode == FIGHT_SEQUENCE::ATM_QUICK_LOW))
	{
		if (GAME_TTYPE::mfIsHuman(pEnemyAvatar->mfType()))
		{
			pEnemyAvatar->mfPlayAnimationOnce(ATTACK1SEQ);
		}
		else
		{
			pEnemyAvatar->mfPlayAnimationOnce(EXPIRESEQ);
		}
		pEnemyAvatar->mfSetType( FIREBALL_1);
		pEnemyAvatar->SetAIFuncIndex( CAvatar::AI_FUNC_FIREBALL );
	}
	else
	{
		pEnemyAvatar->mfPlayAnimationOnce(EXPIRESEQ);
	}
	pEnemyAvatar->Status = AI_DEAD;
	
	pEnemyAvatar->mfClearAllLoopingSnds();
	
	pEnemyAvatar->mfDieSounds();
	
}
/* ========================================================================
   Function    - mfDieSounds
   Description - Die! Die! Die!
   Returns     - void
   ======================================================================== */
void CAvatar::mfDieSounds()
{
	switch(GAME_TTYPE::mfGetSoundType(mfType()))
	{
	case SPIDER_SOUNDS:
		mfQueueSnd(SND_SPIDER_DIE1, SND_SPIDER_DIE_TOTAL, DEATH_SCREAM_FRAME_CNT);
		break;
	case HARPY_SOUNDS:
		mfQueueSnd(SND_HARPY_DIE1, SND_HARPY_DIE_TOTAL, DEATH_SCREAM_FRAME_CNT);
		break;
	case ANKHEG_SOUNDS:
		mfQueueSnd(SND_ANKHEG_DIE1, SND_ANKHEG_DIE_TOTAL, DEATH_SCREAM_FRAME_CNT);
		break;
	case HELL_HOUND_SOUNDS:
		mfQueueSnd(SND_HELLHOUND_DIE1, SND_HELLHOUND_DIE_TOTAL, DEATH_SCREAM_FRAME_CNT);
		break;
	case DOG_SOUNDS:
		mfQueueSnd(SND_DOG_DIE1, SND_DOG_DIE_TOTAL, DEATH_SCREAM_FRAME_CNT);
		break;
	case OGRE_SOUNDS:
		mfQueueSnd(SND_OGRE_DIE1, SND_OGRE_DIE_TOTAL, DEATH_SCREAM_FRAME_CNT);
		break;
	case WYVERN_SOUNDS:
		mfQueueSnd(SND_WYVERN_DIE1, SND_WYVERN_DIE_TOTAL, DEATH_SCREAM_FRAME_CNT);
		break;
	case GOBLIN_SOUNDS:
		mfQueueSnd(SND_GOBLIN_DIE1, SND_GOBLIN_DIE_TOTAL, DEATH_SCREAM_FRAME_CNT);
		break;
	case GNOLL_SOUNDS:
		mfQueueSnd(SND_GNOLL_DIE1, SND_GNOLL_DIE_TOTAL, DEATH_SCREAM_FRAME_CNT);
		break;
	case GORGON_SOUNDS:
		mfQueueSnd(SND_GORGON_DIE1, SND_GORGON_DIE_TOTAL, DEATH_SCREAM_FRAME_CNT);
		break;
	case WRAITH_SOUNDS:
	case SPECTRE_SOUNDS:
		mfQueueSnd(SND_WRAITH_DIE1, SND_WRAITH_DIE_TOTAL, DEATH_SCREAM_FRAME_CNT);
		break;
	case SKELETON_SOUNDS:
		mfQueueSnd(SND_SKELETON_DIE1, SND_SKELETON_DIE_TOTAL, DEATH_SCREAM_FRAME_CNT);
		break;
	case FEMALE_SERVANT_SOUNDS:
	case FEMALE_SOLDIER_SOUNDS:
	case FEMALE_LORD_SOUNDS:
		mfQueueSnd(SND_FEMALE_DIE1, SND_FEMALE_DIE_TOTAL, DEATH_SCREAM_FRAME_CNT);
		break;
	case MALE_SERVANT_SOUNDS:
	case MALE_SOLDIER_SOUNDS:
	case MALE_LORD_SOUNDS:
	case ZOMBIE_SOUNDS:
		mfQueueSnd(SND_MALE_DIE1, SND_MALE_DIE_TOTAL, DEATH_SCREAM_FRAME_CNT);
		break;
	case GARGOYLE_SOUNDS:
		mfQueueSnd(SND_CRUMBLE1, SND_CRUMBLE_TOTAL, DEATH_SCREAM_FRAME_CNT);
		break;
	default:	// No die sound for this type.
		break;
	}
}
/* ========================================================================
   Function	- mfFightSounds
   Description - Make clanging noises apropriate to the armor and weapon.
   Returns	 -
   ======================================================================== */

void CAvatar::mfFightSounds(HDL_AVATAR const hEnemyTarget)
{
	BOOL bHitOrNot = FALSE;
	
	if (hEnemyTarget == fERROR)
	{
		return;
	}
	
	//GEH SetBlockAttr(hEnemyTarget, LOCKED, LOCKED);
	CAvatar * const pEnemyTarget = (CAvatar * const) BLKPTR(hEnemyTarget);
	
	if (pEnemyTarget->mfAmIImmoblized())
	{
		// Hey you're hitting a dead thing.
		mfSethEnemy(fERROR);
		return;
	}
	
	if (pEnemyTarget->hPlayerStats != fERROR && hPlayerStats != fERROR)
	{
		//GEH SetBlockAttr(hPlayerStats, LOCKED, LOCKED);
		//GEH SetBlockAttr(pEnemyTarget->hPlayerStats, LOCKED, LOCKED);
		DumbAutoLockPtr<PLAYER_STATS> const pAvatarStats(hPlayerStats);
		DumbAutoLockPtr<PLAYER_STATS> const pEnemyStats(pEnemyTarget->hPlayerStats);
		SBYTE AttackRoll;
		
		
		PLAYER_HIT const PlayerHitObj(pAvatarStats, pEnemyStats);
		
		LONG const Modifier = mfCalculateAttackModifier(hEnemyTarget);
		bHitOrNot = PlayerHitObj.mfRollAttackTarget( Modifier,
											   &AttackRoll);
		
		if (bHitOrNot == TRUE)
		{
			mfFightHitSounds(pAvatarStats, pEnemyTarget, pEnemyStats);
		}
		else
		{
			// We missed.
		    mfFightMissSounds(pAvatarStats, pEnemyTarget, pEnemyStats);
		}
		
		//GEH ClrLock(pEnemyTarget->hPlayerStats);
		//GEH ClrLock(hPlayerStats);
	}
	else
	{
	   // The Enemy has no player stats so just clang
	   	
	   switch(GAME_TTYPE::mfGetSoundType(pEnemyTarget->mfType()))
	   {
		case WYVERN_SOUNDS:
		case ANKHEG_SOUNDS:
			AddSndObj(SND_BONE_CRACKLE1, SND_BONE_CRACKLE_TOTAL, ThingIndex);
			break;
	   	case DOG_SOUNDS:
			AddSndObj(SND_DOG_BARK1, SND_DOG_BARK_TOTAL, ThingIndex);
			break;
	   	default:
			AddSndObj(SND_BIG_SWORD_HIT1, SND_BIG_SWORD_HIT_TOTAL, ThingIndex);
			break;
		}
	}
	
	//GEH ClrLock(hEnemyTarget);
}
/* ========================================================================
   Function	- mfFightMissSounds
   Description - Make clanging noises apropriate to the armor and weapon.
   Returns	 -
   ======================================================================== */

void CAvatar::mfFightMissSounds(PLAYER_STATS const * const pAvatarStats,
								CAvatar * const pEnemyTarget,
								PLAYER_STATS const * const pEnemyStats) const
{
	// We missed.
    switch(pAvatarStats->mfGetFirstWeapon())
    {
	case WEAPON_INFO::NO_WEAPON:
		// No noise
		break;
    case WEAPON_INFO::BATTLE_AXE:
	   	pEnemyTarget->mfQueueSnd(SND_AX_CHOP_SHIELD1, SND_AX_CHOP_SHIELD_TOTAL, HIT_FRAME_CNT);
    	break;
    case WEAPON_INFO::CLUB_WAR:
    case WEAPON_INFO::QUARTER_STAFF:
		pEnemyTarget->mfQueueSnd(SND_LEATHER_ARMOR_HIT1, SND_LEATHER_ARMOR_HIT_TOTAL, HIT_FRAME_CNT);
    	break;
	default:
		switch(pEnemyStats->mfGetCurrentArmor())
		{
		case ARMOR_INFO::NO_ARMOR:
		case ARMOR_INFO::HIDE:
		case ARMOR_INFO::LEATHER:
		case ARMOR_INFO::PADDED:
		case ARMOR_INFO::STUDDED_LEATHER:
			// No noise;
			break;
		case ARMOR_INFO::CHAIN_MAIL:
		case ARMOR_INFO::BANDED_MAIL:
		case ARMOR_INFO::IMPROVED_MAIL:
		case ARMOR_INFO::PLATE_MAIL:
		case ARMOR_INFO::RING_MAIL:
		case ARMOR_INFO::SCALE_MAIL:
		case ARMOR_INFO::SPLINT_MAIL:
			pEnemyTarget->mfQueueSnd(SND_LEATHER_ARMOR_HIT1, SND_LEATHER_ARMOR_HIT_TOTAL, HIT_FRAME_CNT);
			break;
		default:
			switch(random(2))
			{
			case 0:
				pEnemyTarget->mfQueueSnd(SND_MUTE_SWORD_HIT1, SND_MUTE_SWORD_HIT_TOTAL, HIT_FRAME_CNT);
				break;
			case 1:
				pEnemyTarget->mfQueueSnd(SND_SHIELD_HIT1, SND_SHIELD_HIT_TOTAL, HIT_FRAME_CNT);
				break;
			}
			break;
		}
		break;
	}
}
/* ========================================================================
   Function	- mfFightStartSounds
   Description - Make swish noises apropriate to the weapon.
   Returns	 -
   ======================================================================== */

void CAvatar::mfFightStartSounds(PLAYER_STATS const * const pAvatarStats) const
{
	// Make an attack noise.
	switch(GAME_TTYPE::mfGetSoundType(mfType()))
	{
	case DOG_SOUNDS:
		switch(random(2))
		{
		case 0:
			AddSndObj(SND_DOG_BARK1, SND_DOG_BARK_TOTAL, ThingIndex);
			break;
		case 2:
			AddSndObj(SND_WOLF_SNARL1, SND_WOLF_SNARL_TOTAL, ThingIndex);
			break;
		}
		break;
	case WYVERN_SOUNDS:
		AddSndObj(SND_WYVERN_SCREAM1, SND_WYVERN_SCREAM_TOTAL, ThingIndex);
		break;
	case SPIDER_SOUNDS:
	case SPIDER_KING_SOUNDS:
		AddSndObj(SND_SPIDER_SPIT1, SND_SPIDER_SPIT_TOTAL, ThingIndex);
		break;
	case OTYUGH_SOUNDS:
	case ANKHEG_SOUNDS:
	case SPECTRE_SOUNDS:
	case WRAITH_SOUNDS:
		// Make no noise for their attack.
		break;
	default:
		switch(pAvatarStats->mfGetFirstWeapon())
		{
		case WEAPON_INFO::NO_WEAPON:
			break;
		case WEAPON_INFO::BOW:
			AddSndObj(SND_ARROW_SHOT1, SND_ARROW_SHOT_TOTAL, ThingIndex);
			break;
		case WEAPON_INFO::CROSSBOW_LT:
		case WEAPON_INFO::CROSSBOW_HVY:
			AddSndObj(SND_CROSSBOW_SHOT1, SND_CROSSBOW_SHOT_TOTAL, ThingIndex);
			break;
		default:
			// Kind of swooosh noise of thing swinging in the air.
			AddSndObj(SND_SWORD_SLASH1, SND_SWORD_SLASH_TOTAL, ThingIndex);
			break;
		}
		break;
	}
	
}
/* ========================================================================
   Function	- mfFightHitSounds
   Description - Make hit noises apropriate to the armor and weapon.
   Returns	 -
   ======================================================================== */

void CAvatar::mfFightHitSounds(PLAYER_STATS const * const pAvatarStats,
	                           CAvatar * const pEnemyTarget,
	                           PLAYER_STATS const * const pEnemyStats) const
{
	switch(GAME_TTYPE::mfGetSoundType(mfType()))
	{
	case WYVERN_SOUNDS:
	case ANKHEG_SOUNDS:
		// GWP remove ifdef
		switch(random(3))
		{
		case 0:
			pEnemyTarget->mfQueueSnd(SND_BONE_BREAK1, SND_BONE_BREAK_TOTAL, HIT_FRAME_CNT);
			break;
		case 1:
			pEnemyTarget->mfQueueSnd(SND_BONE_CRUSH1, SND_BONE_CRUSH_TOTAL, HIT_FRAME_CNT);
			break;
		case 2:
			pEnemyTarget->mfQueueSnd(SND_BONE_CRACKLE1, SND_BONE_CRACKLE_TOTAL, HIT_FRAME_CNT);
			break;
	    }
	    break;
    default:
	    switch(pAvatarStats->mfGetFirstWeapon())
	    {
		case WEAPON_INFO::NO_WEAPON:
			switch(random(4))
			{
			case 0:
				pEnemyTarget->mfQueueSnd(SND_BONE_BREAK1, SND_BONE_BREAK_TOTAL, HIT_FRAME_CNT);
				break;
			case 1:
				pEnemyTarget->mfQueueSnd(SND_BONE_CRUSH1, SND_BONE_CRUSH_TOTAL, HIT_FRAME_CNT);
				break;
			case 2:
				pEnemyTarget->mfQueueSnd(SND_BONE_CRACKLE1, SND_BONE_CRACKLE_TOTAL, HIT_FRAME_CNT);
				break;
			case 3:
				pEnemyTarget->mfQueueSnd(SND_LEATHER_ARMOR_HIT1, SND_LEATHER_ARMOR_HIT_TOTAL, HIT_FRAME_CNT);
				break;
		    }
			break;
		case WEAPON_INFO::CROSSBOW_LT:
		case WEAPON_INFO::CROSSBOW_HVY:
	    case WEAPON_INFO::BOW:
		   	pEnemyTarget->mfQueueSnd(SND_ARROW_HIT1, SND_ARROW_HIT_TOTAL, HIT_FRAME_CNT);
		   	break;
	    case WEAPON_INFO::BATTLE_AXE:
		   	pEnemyTarget->mfQueueSnd(SND_AX_CHOP_FLESH1, SND_AX_CHOP_FLESH_TOTAL, HIT_FRAME_CNT);
	    	break;
	    case WEAPON_INFO::CLUB_WAR:
	    case WEAPON_INFO::QUARTER_STAFF:
			pEnemyTarget->mfQueueSnd(SND_LEATHER_ARMOR_HIT1, SND_LEATHER_ARMOR_HIT_TOTAL, HIT_FRAME_CNT);
	    	break;
	    case WEAPON_INFO::HALBERD:
	    case WEAPON_INFO::LANCE:
	    case WEAPON_INFO::ONE_HAND_SPEAR:
	    case WEAPON_INFO::TWO_HAND_SPEAR:
			pEnemyTarget->mfQueueSnd(SND_WEAPON_STAB1, SND_WEAPON_STAB_TOTAL, HIT_FRAME_CNT);
	    	break;
	    default:
			switch(pEnemyStats->mfGetCurrentArmor())
			{
			case ARMOR_INFO::NO_ARMOR:
				switch(GAME_TTYPE::mfGetSoundType(pEnemyTarget->mfType()))
				{
				// You hit an Ankheg or Skeleton.
				case ANKHEG_SOUNDS:
				case SKELETON_SOUNDS:
					pEnemyTarget->mfQueueSnd(SND_BONE_BREAK1, SND_BONE_BREAK_TOTAL, HIT_FRAME_CNT);
					break;
				default:
					pEnemyTarget->mfQueueSnd(SND_LEATHER_ARMOR_HIT1, SND_LEATHER_ARMOR_HIT_TOTAL, HIT_FRAME_CNT);
					break;
				}
				break;
			case ARMOR_INFO::HIDE:
			case ARMOR_INFO::LEATHER:
			case ARMOR_INFO::PADDED:
			case ARMOR_INFO::STUDDED_LEATHER:
				pEnemyTarget->mfQueueSnd(SND_LEATHER_ARMOR_HIT1, SND_LEATHER_ARMOR_HIT_TOTAL, HIT_FRAME_CNT);
				break;
			case ARMOR_INFO::CHAIN_MAIL:
			case ARMOR_INFO::BANDED_MAIL:
			case ARMOR_INFO::IMPROVED_MAIL:
			case ARMOR_INFO::PLATE_MAIL:
			case ARMOR_INFO::RING_MAIL:
			case ARMOR_INFO::SCALE_MAIL:
			case ARMOR_INFO::SPLINT_MAIL:
				pEnemyTarget->mfQueueSnd(SND_CHAIN_ARMOR_HIT1, SND_CHAIN_ARMOR_HIT_TOTAL, HIT_FRAME_CNT);
				break;
			// case ARMOR_INFO::BREASTPLATE:
			// case ARMOR_INFO::BRONZE_PLATE:
			// case ARMOR_INFO::FIELD_PLATE:
			// case ARMOR_INFO::FULL_PLATE:
			// case ARMOR_INFO::HALF_PLATE:
				// Intentional fall thru.
			default:
				switch(random(2))
			    {
				case 0:
				    switch(WEAPON_INFO::mfGetSize(pAvatarStats->mfGetFirstWeapon()))
				    {
				    case WEAPON_INFO::SMALL:
				    case WEAPON_INFO::MEDIUM:
						pEnemyTarget->mfQueueSnd(SND_SMALL_SWORD_HIT1, SND_SMALL_SWORD_HIT_TOTAL, HIT_FRAME_CNT);
				    case WEAPON_INFO::LARGE:
				    case WEAPON_INFO::HUGE:
						pEnemyTarget->mfQueueSnd(SND_BIG_SWORD_HIT1, SND_BIG_SWORD_HIT_TOTAL, HIT_FRAME_CNT);
						break;
					}
					break;
				case 1:	
					pEnemyTarget->mfQueueSnd(SND_METAL_ARMOR_HIT1, SND_METAL_ARMOR_HIT_TOTAL, HIT_FRAME_CNT);
					break;
				}
				break;
			}
			break;
		}
		break;
	}
}

/* ========================================================================
   Function	- mfTurnToSpectre
   Description - turn an avatar to be a spectre
   Returns	 -
   ======================================================================== */
void CAvatar::mfTurnToSpectre()			
{
	PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	LONG i=CAvatar::CreateAvatar((LONG)ThingType, ThingType, mfX(),
						  mfY(), mfAngle(),
						  AI_FUNC_SPECTRE, pCurrentScene,
						  REALM::MONSTERS);
	CAvatar * pNewAvatar = (CAvatar *) BLKPTR (pCurrentScene->Avatars[i]);
	if (fERROR != (pNewAvatar->hPlayerStats = LoadStats((LONG)LESSER_WRAITH,pCurrentScene->Avatars[i])))
	{
		PLAYER_STATS *pPlayerStats;
		pPlayerStats = (PLAYER_STATS*)BLKPTR(pNewAvatar->hPlayerStats);
		pPlayerStats->mfSetType(ThingType);
	}
	pNewAvatar->ThingType = ThingType;
	pNewAvatar->mfSetOriginalRemapColor(WRAITH_REMAP);
	pNewAvatar->bumpTimer = 8;
}

/* ========================================================================
   Function	- mfFallingDamage
   Description - apply falling damage to Avatar
   Returns     - TRUE if player still alive after fall, FALSE if not.
	              [d4-18-97, 4-25-97 JPC] new return value to fix bug where leader
					  dies in a fall and follower takes no damage from fall.
	Notes     	- Called from FollowPlayer in AIFOLPLY.CPP.
   ======================================================================== */
BOOL CAvatar::mfFallingDamage (WadThingType bump, LONG floor, LONG fallHeight)
{
// bump = what you bumped into
// floor = floor height where you used to be.
// ---------------------------------------------------------------------------

	LONG			hits;
	LONG			i;
	LONG			vulFactor = 100;
	BOOL			retval = TRUE;	// [d4-18-97, 4-25-97 JPC] default = did not die

	if (attrib.IsFeathered || attrib.IsJumpEnhanced)
		return TRUE;							// [d4-18-97, 4-25-97 JPC] did not die

	if (hPlayerStats != fERROR)
	{
		DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hPlayerStats);
		if (pPlayerStats != NULL)
		{
			vulFactor =  pPlayerStats->mfDamageVulFactor();
			if (vulFactor <= 0)
				return TRUE;					// [d4-18-97, 4-25-97 JPC] invulnerable, did not die
		}

		hits = 1;								// take at least 1 hit point

		// Take 1 die 6 hits for every 10 feet you fall, then adjust
		// for type of landing sector.
		fallHeight /= FALL_DIVISOR;		// convert pixels to 10-foot units
		for (i = 0; i < fallHeight; ++i)
		{
			hits += (random (6) + 1);
		}

		switch (bump)
		{
			case iSHALLOW_WATER:
			case iACID:
				hits /= 2;
				break;
			case iDEEP_WATER:
			case iLAVA:
				hits /= 4;
				break;
		}

		hits = (hits * vulFactor) / 100;

		pPlayerStats->mfTakeDamage( hits);

		if (pPlayerStats->mfGetCurHitPoints() <= 0)
		{
			pPlayerStats->mfKillem();
			if (hEnemy != fERROR)
			{
				mfSethEnemy(fERROR);
			}
			mfPlayAnimationOnce(EXPIRESEQ);
			Status = AI_DEAD;
         retval = FALSE;               // [d4-18-97, 4-25-97 JPC] did die
			mfDieSounds();
		}
		else
		if (hits > 3)
		{
			mfPainSounds();
		}
	}
	else
	{
		if (hEnemy != fERROR)
		{
			mfSethEnemy(fERROR);
		}
		
		mfPlayAnimationOnce(EXPIRESEQ);
		Status = AI_DEAD;
		
	    mfDieSounds();
	}
	return retval;								// [d4-18-97, 4-25-97 JPC]
}


/* ========================================================================
   Function		- GetSectorDamage
   Description	- compute damage to Avatar from being in this sector
   Returns		- FALSE if no damage, TRUE if any damage, in which case
				  the parameter "damageNumber" returns the hit points.
   Notes		- This function used to live in SECTORS.C.
   ======================================================================== */
BOOL GetSectorDamage (LONG iSector, LONG *damageNumber, CAvatar * pAvatar,BOOL* pfDamagedByLava)
{
	LONG			damage;
	LONG			sectorHeight;
	LONG 			avatarHeight = pAvatar->mfHeight();
	LONG 			avatarZ = pAvatar->mfZ();
	LONG			vulFactor = 100;			// invulnerability is not either/or;
													// it is a percentage
	*pfDamagedByLava=FALSE;

	PLAYER_STATS *pPlayerStats;

	if (pAvatar->hPlayerStats != fERROR)
		pPlayerStats = (PLAYER_STATS*)BLKPTR(pAvatar->hPlayerStats);
	else
		pPlayerStats = NULL;

	damage = 0;
	sectorHeight = sectors[iSector].ch - sectors[iSector].fh;

	// Simple rule: once the ceiling touches the avatar's head, the
	// avatar is dead.  We can make this more complicated if we want to.
	if (sectorHeight < avatarHeight)
	{
		// Check for various crushers.
		// [d2-21-97 JPC] Changed crushers to hurt severely, but not kill.
		if (FloorIsActive (iSector) || CeilingIsActive (iSector))
		{
			if (pPlayerStats != NULL)
            damage = pPlayerStats->mfGetCurHitPoints() - 1; // should be almost but not quite dead
		}
	}

	// Hand out special damage every DAMAGE_FRAME frames.
	if ((gcFrames % DAMAGE_FRAME) == 0)
	{
		switch (sectors[iSector].special)
		{
			case SSP_DAMAGE_2:
			case SSP_DAMAGE_5:
			case SSP_DAMAGE_10:
				// [d11-26-96 JPC] The above types should not have been used,
				// but if they were, treat them like lava.  (Previous damage
				// amounts were way too high.)
			case SSP_LAVA:
				if (avatarZ <= sectors[iSector].fh)
				{
					// JPC: All team members get this mod when we activate the item,
					// so all we have to do is check it for this player.
					// Fire resistance should also be a percentage but it is always
					// 100 or 0 (as of 12-02-96), so leave the following alone.
					if (pPlayerStats != NULL && !pPlayerStats->mfAmFireResistant())
					{
						*pfDamagedByLava=TRUE;
						damage += LAVA_DAMAGE;
					}
				}
				break;
			case SSP_POISON_GAS:
				damage += POISON_DAMAGE;
				break;
			case SSP_ACID_FLOOR:
				if (avatarZ <= sectors[iSector].fh)
					damage += ACID_DAMAGE;
				break;
		}
	}

	if (pPlayerStats != NULL)
		vulFactor =  pPlayerStats->mfDamageVulFactor();

	if (vulFactor < 0)
		vulFactor = 0;

	damage = (damage * vulFactor) / 100;

	*damageNumber = damage;

	if (damage)
		return TRUE;
	else
		return FALSE;
}

/* ========================================================================
   Function		- mfCheckCrush
   Description - apply crushing damage to Avatar
   Returns	 	-
	Notes			- applies all sector damage, such as lava and acid;
					  not just for crushers.
   ======================================================================== */
void CAvatar::mfCheckCrush()
{
	// No damage if we are just standing waiting for commands.
	if (Status == AI_PAUSED || Status == AI_BEGIN_PAUSE)
		return;
		
	LONG damageNumber;
	SHORT crush=FALSE;
	LONG sector=mfGetSector();
	BOOL fDamagedByLava=FALSE;
	
	// crush = GetCrusherDamage(sector, &damageNumber,mfHeight(), mfZ());
	crush = GetSectorDamage(sector, &damageNumber, this, &fDamagedByLava);
	if(crush)
	{
		if (hPlayerStats != fERROR)
		{
			DumbAutoLockPtr<PLAYER_STATS> const pPlayerStats(hPlayerStats);
			
			pPlayerStats->mfTakeDamage( damageNumber);
	
			if (pPlayerStats->mfGetCurHitPoints() <= 0)
			{
				pPlayerStats->mfKillem();
				if (hEnemy != fERROR)
				{
					mfSethEnemy(fERROR);
				}
				if (fDamagedByLava)
					attrib.KilledByLava=TRUE;
	
				mfPlayAnimationOnce(EXPIRESEQ);
				Status = AI_DEAD;
				mfDieSounds();
			}
		}
		else
		{
			if (hEnemy != fERROR)
			{
				mfSethEnemy(fERROR);
			}
			
			mfPlayAnimationOnce(EXPIRESEQ);
			Status = AI_DEAD;
		    mfDieSounds();
		}
	}
}

/* ========================================================================
   Function    - mfGetWalkRate
   Description - returns the modified walk rate
   Returns     - a movement rate
   ======================================================================== */
// Get the walking rate for this avatar.
const MOVEMENT_RATE CAvatar::mfGetWalkRate() const
{
	MOVEMENT_RATE MR = GAME_TTYPE::mfGetWalkRate(mfType());
	
	if(hPlayerStats != fERROR)
	{
		PLAYER_STATS const *const pPlayerStats = (PLAYER_STATS const * const)BLKPTR(hPlayerStats);
		// if speed enhanced
		if(pPlayerStats->mfSpeedEnhanced())
			MR *= 2;
	}
		
	return MR;
}

/* ========================================================================
   Function    - mfGetMarchRate
   Description - returns the modified marching rate
   Returns     - a movement rate
   ======================================================================== */
// Get the marching rate for this avatar.
const MOVEMENT_RATE CAvatar::mfGetMarchRate() const
{
	MOVEMENT_RATE MR = GAME_TTYPE::mfGetMarchRate(mfType());
	
	if(hPlayerStats != fERROR)
	{
		PLAYER_STATS const * const pPlayerStats = (PLAYER_STATS const * const)BLKPTR(hPlayerStats);
		
		// if speed enhanced
		if(pPlayerStats->mfSpeedEnhanced())
			MR *= 2;
	}
		
	return MR;
}

/* ========================================================================
   Function    - mfSetKillType
   Description - stores the type of kill someone placed on us
   Returns     - BOOL TRUE already killed
   ======================================================================== */
BOOL CAvatar::mfSetKillType(CAvatar *pAvatar, THINGTYPE Type, LONG DamageLevel)
{
	if(pAvatar->KillType != NO_THING )
		return TRUE; // previous kill
		
	pAvatar->KillType = Type;
	pAvatar->fDamage = DamageLevel;
	return FALSE; // no previous kills
}

/* ========================================================================
   Function    - mfBeginPause
   Description - Begin a pause sequence. Wait for the next command.
   Returns     -
   ======================================================================== */
void CAvatar::mfBeginPause()
{
    // First time thru save your state.
    if (Status != AI_BEGIN_PAUSE &&
    	Status != AI_PAUSED &&
    	Status != AI_END_PAUSE)
    {
	    fPrev.Status = Status;
	    fPrev.Sequence = CurSequence;
    }

    if (!mfAmIImmoblized())
    {
	    if ((CurSequence == ATTACK1SEQ ||
	         CurSequence == ATTACK2SEQ ||
	         CurSequence == DEFENDSEQ) && !mfTestSequenceDone())
	    {
			// Stay in this state until the avatar finishes their swing.
			Status = AI_BEGIN_PAUSE;
	    }
	    else
	    {
		    // If under attack, keep the weapon at the ready.
		    if (Status == AI_DEFEND ||
		        Status == AI_ATTACK )
		    {
			    if (GAME_TTYPE::mfCanFly(mfType()) && attrib.IsFlying)
			    {
				    mfStartAnimationLoop(MARCHSEQ);
			    }
			    else
			    {
				    SetFrame(ThingIndex, ATTACK1SEQ, 1);
			    }
		    }
		    else
		    {
			    if (CurSequence == CROUCHSEQ)
			    {
			    	// if crawling, just freeze in place, don't stand up!
			    	mfSetFrozen(TRUE);
			    }
			    else
			    {
				    if (GAME_TTYPE::mfCanFly(mfType()) && attrib.IsFlying)
				    {
					    mfStartAnimationLoop(WALKSEQ);
				    }
				    else
				    {
					    mfPlayAnimationOnce(STANDSEQ);
				    }
			    }
		    }
		    Status = AI_PAUSED;
		}
	}
}

/* ========================================================================
   Function    - mfSetAttackSequence
   Description - Switch the art for avatar
   Returns     -
   ======================================================================== */
void CAvatar::mfSetAttackSequence(FIGHT_SEQUENCE::ATTACK_MODE const Mode)
{
	switch (Mode)
	{
	case FIGHT_SEQUENCE::ATM_DEFEND:
		if (Status == AI_PAUSED || Status == AI_BEGIN_PAUSE)
		{
			fPrev.Sequence = DEFENDSEQ;
		}
		else
		{
			CurSequence = DEFENDSEQ;
		}
		break;
	case FIGHT_SEQUENCE::ATM_HIGH:
		if (Status == AI_PAUSED || Status == AI_BEGIN_PAUSE)
		{
			fPrev.Sequence = ATTACK1SEQ;
		}
		else
		{
			CurSequence = ATTACK1SEQ;
		}
		break;
	case FIGHT_SEQUENCE::ATM_LOW:
		if (Status == AI_PAUSED || Status == AI_BEGIN_PAUSE)
		{
			if (GAME_TTYPE::mfHasAttack2Animation(mfType()))
				fPrev.Sequence = ATTACK2SEQ;
			else
				fPrev.Sequence = ATTACK1SEQ;
		}
		else
		{
			if (GAME_TTYPE::mfHasAttack2Animation(mfType()))
				CurSequence = ATTACK2SEQ;
			else
				CurSequence = ATTACK1SEQ;
		}
		break;
	case FIGHT_SEQUENCE::ATM_QUICK_HIGH:
		if (Status == AI_PAUSED || Status == AI_BEGIN_PAUSE)
		{
			fPrev.Sequence = ATTACK1SEQ;
		}
		else
		{
			Status = AI_ATTACK;
			CurSequence = ATTACK1SEQ;
		}
		break;
	case FIGHT_SEQUENCE::ATM_QUICK_LOW:
		if (Status == AI_PAUSED || Status == AI_BEGIN_PAUSE)
		{
			if (GAME_TTYPE::mfHasAttack2Animation(mfType()))
				fPrev.Sequence = ATTACK2SEQ;
			else
				fPrev.Sequence = ATTACK1SEQ;
		}
		else
		{
			if (GAME_TTYPE::mfHasAttack2Animation(mfType()))
				CurSequence = ATTACK2SEQ;
			else
				CurSequence = ATTACK1SEQ;
		}
		break;
	case FIGHT_SEQUENCE::ATM_MAGIC:
		if (Status == AI_PAUSED || Status == AI_BEGIN_PAUSE)
		{
			if (GAME_TTYPE::mfHasMagicAnimation(mfType()))
				fPrev.Sequence = MAGICSEQ;
			else
				fPrev.Sequence = ATTACK1SEQ;
		}
		else
		{
			if (GAME_TTYPE::mfHasMagicAnimation(mfType()))
				CurSequence = MAGICSEQ;
			else
				CurSequence = ATTACK1SEQ;
		}
		break;
	}
}
/* ========================================================================
   Function    - mfCanSeeAvatar
   Description - inline Check whether the Avatar is looking at another avatar.
   Returns     - TRUE | FALSE
   ======================================================================== */
const BOOL CAvatar::mfCanSeeAvatar(const CAvatar *pTarget) const
{
	LONG distance, HearingDistance = 100;
	//FIXED_VECTOR_3D	TempVector;
	PLAYER	TempPlayer;
	LONG Angle;
	LONG BumpDistance;
	
	
	// should we concider this guy at all
	if(!reject( mfGetSector(), pTarget->mfGetSector()))
		return FALSE;
	
	// Is he too far to hear him
	distance = aprox_dist(mfX(), mfY(), pTarget->mfX(), pTarget->mfY());
	if(distance > HearingDistance)
	{
		// check to see if I am facing that way
		LONG AngleToTarget = AngleFromPoint(mfX(), mfY(),
		                                    pTarget->mfX(),
		                                    pTarget->mfY(),
		                                    RESOLUTION_4);
	    // Normalize the angle
	    AngleToTarget -= mfAngle();
	    if (AngleToTarget < 0)
	    {
	    	AngleToTarget += 256;
	    }
	    // Is the Avatar looking at the Target?
	    if (!(AngleToTarget <= 72 ||
	          AngleToTarget >= 184))
	    {
	    	return(FALSE);
	    }
	}
	
	// now check a line of sight calculation
	// Test line to this guy
	//TempVector.dx = 0;
	//TempVector.dy = distance << PLAYER_FIXEDPT;
	//TempVector.dz = 0;
	Angle = AngleFromPoint2 ( mfX(), mfY(), pTarget->mfX(), pTarget->mfY(), RESOLUTION_1);
	//Rotate((POINT *)&TempVector, Angle);
	
	TempPlayer.x = mfX() << PLAYER_FIXEDPT;
	TempPlayer.y = mfY() << PLAYER_FIXEDPT;
	TempPlayer.z = mfZ() + 32;
	TempPlayer.p = 0;
	TempPlayer.a = mfAngle();
	TempPlayer.Flying = TRUE;
	TempPlayer.w = 16;
	TempPlayer.h = 16;
	TempPlayer.bump = iNOTHING;
	TempPlayer.ThingIndex = fERROR;
	
	CheckLongMove(&TempPlayer,
	          Angle,
			  distance,
	          0,
	          pTarget->mfZ(),
	          &BumpDistance);
	
    // if I hit something, los is blocked
    switch (TempPlayer.bump)
    {
    case iNOTHING:
	 case iSHALLOW_WATER:
	 case iDEEP_WATER:
	 case iLAVA:
	 case iACID:
    	break;
	case iFLOOR:
		if (GAME_TTYPE::mfCanFly(mfType()))
			return (TRUE);
    default:
    	return (FALSE);
    }
	
	return (TRUE);
}

/* ========================================================================
   Function    - mfTurnToStone
   Description - Turn this avatar into stone.
   Returns     -
   ======================================================================== */
void CAvatar::mfTurnToStone()
{

	if(ThingType != SPECTRE && ThingType != T_SPECTRE
	&& ThingType != WRAITH  && ThingType != T_WRAITH)
	{
		LONG FloorHeight;
		LONG CeilingHeight;
		LONG Special;
		
		// for the moment, freeze the Avatar.
		mfSetOriginalRemapColor(GRAYSCALE_REMAP);
		mfSetFrozen(TRUE);
	    fPrev.Status = Status;
	    Status = AI_STONE;
	    if (hEnemy != fERROR)
	    {
	    	// Disengage!
	    	mfSethEnemy(fERROR);
	    }

		if(ThingType ==	HARPY || ThingType == T_HARPY)
			mfClearLoopingSnd(SND_HARPY_WINGFLAP1);
		else if(ThingType == WYVERN_1 || ThingType == T_WYVERN_1)
			mfClearLoopingSnd(SND_WYVERN_WINGFLAP1);
		else if(ThingType == WYVERN_2 || ThingType == T_WYVERN_2)
			mfClearLoopingSnd(SND_SMALL_WYVERN_WINGFLAP1);

		mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
		if(mfZ() != FloorHeight)
			mfMoveToXYZA(mfX(), mfY(), FloorHeight, mfAngle());

	}
}

#if 0 // UNUSED
/* ========================================================================
   Function    - mfWriteSceneData
   Description - Write to the scene file the current state of the Avatar's AI and any info
				it needs.
   Returns     -
   ======================================================================== */
void CAvatar::mfWriteSceneData(FILE * const fp)
{
	if (mfShouldSaveToSceneFile())
	{
		fprintf(fp, "%ld %ld %ld %ld %ld %ld\n",
			Id,
			mfX(),
			mfY(),
			mfZ(),
			mfAngle(),
			GetAIFuncIndex()
			);
			
		switch(GetAIFuncIndex())
		{
		case AI_FUNC_FOLLOW_PATH:
			fFollowPath.mfWriteData(fp);
			break;
		case AI_FUNC_FOLLOW_ID:
			fFollowId.mfWriteData(fp);
			break;
		case AI_FUNC_GUARD_POINT:
			fGuardPt.mfWriteData(fp);
			break;
		case AI_FUNC_PING_PONG:
			fPingPong.mfWriteData(fp);
			break;
		case AI_FUNC_BATTLE_CAPTAIN:
			fBtlCap.mfWriteData(fp);
			break;
		case AI_FUNC_FIND_TYPE:
			fFindType.mfWriteData(fp);
			break;
		case AI_FUNC_FOLLOW_PLAYER:
			fFollowPlayer.mfWriteData(fp);
			break;
		case AI_FUNC_PATH_LEADER:
			fPathLeader.mfWriteData(fp);
			break;
		case AI_FUNC_PATH_ATTACK:
			fPathAttack.mfWriteData(fp);
			break;
		}
	}
	
}
#endif // 0

/* ========================================================================
   Function    - mfRotateToCamera
   Description - Wait until you are facing the camera and then go to the next phase.
   Returns     - TRUE | FALSE
   ======================================================================== */
void CAvatar::mfRotateToCamera()
{
    if ((SCENE_MGR::gTick - fPrev.TimeLastRotate) > 1) //GWP phoney time count.
    {
	    // Rotate at a reasonable speed.
	    const LONG NewAngle = AngleFromPoint(mfX(),
	                                         mfY(),
	                                         CAMERA_INT_VAL(camera.x),
	                                         CAMERA_INT_VAL(camera.y),
	                                         RESOLUTION_8);
	
	    fPrev.TimeLastRotate = SCENE_MGR::gTick;
	    const LONG A = mfRotateToward(mfAngle(), NewAngle);
	
	    // Are we facing the camera yet?
	    if (A == NewAngle)
	    {
			Status = AI_BEGIN_LISTEN_BOW;
			mfStartAnimationOnce(BOWSEQ); // Bow
	    }
    }
}

/* ========================================================================
   Function    - mfAmIBeingAttacked
   Description - Test whether this avatar is under attack or not.
   Returns     - TRUE or FALSE
   ======================================================================== */

BOOL const CAvatar::mfAmIBeingAttacked() const
{
	return (Engaged > 0) ? TRUE : FALSE;
}

/* ========================================================================
   Function    - mfAmICastingASpell
   Description - Test whether this avatar is Casting a spell or not.
   Returns     - TRUE or FALSE
   ======================================================================== */

BOOL const CAvatar::mfAmICastingASpell() const
{
	return (Status == AI_CASTSPELL);
}

/* ========================================================================
   Function    - mfAmIChasingAnEnemy
   Description - Test whether this avatar is chasing someone.
   Returns     -
   ======================================================================== */

BOOL const CAvatar::mfAmIChasingAnEnemy() const
{
	return ((Status == AI_MOVING || Status == AI_ATTACK) &&
	        hEnemy != fERROR);
}

/* ========================================================================
   Function    - mfAmIAlive
   Description - Test whethter the avatar is alive or not
   Returns     - TRUE or FALSE
   ======================================================================== */

BOOL const CAvatar::mfAmIAlive() const
{
	return (!mfAmIImmoblized());
}

/* ========================================================================
   Function    - mfIfAliveRestoreHealth
   Description - At the end of the adventure, if you survived you get back
                 to your full hit pts.
   Returns     -
   ======================================================================== */

void CAvatar::mfIfAliveRestoreHealth()
{
	if (!mfAmIImmoblized())
	{
		if (hPlayerStats != fERROR)
		{
			PLAYER_STATS *const pPlayerStats = (PLAYER_STATS * const) BLKPTR(hPlayerStats);
			pPlayerStats->mfRestoreFullHitPts();
		}
	}
}

#if 0 // UNUSED
/* ========================================================================
   Function    - mfSpewAll
   Description -  Drop this avatar's stuff all over the place.
				  currently, it just drops it all at his feet...
   Returns     -
   ======================================================================== */

void CAvatar::mfSpewAll()
{
	if (hPlayerStats == fERROR)
		return;

	DumbAutoLockPtr<PLAYER_STATS>const pPlayerStats(hPlayerStats);
	InventoryItor itor(pPlayerStats->Inventory);

	itor = pPlayerStats->Inventory.begin();

	while (itor != pPlayerStats->Inventory.end())
	{
		if (( (*itor)->mfCategory() == IC_SPELL) ||
			( (*itor)->mfCategory() == IC_REALM_SPELL) ||
			( (*itor)->mfCategory() == IC_BLOODAB))
		{
			itor++;
		}
		else
		{
			//the last argument determines whether or not to put it on top
			//of the person...  useful for when the bodies are dead...
			//totally silly if they're still alive, because the item would
			//appear on top of their head.	

			itor.mfDropItem(mfX(), mfY(),TRUE);  // implicit advance... bleh
		}
	}
}
#endif
#if 0 // UNUSED
/* ========================================================================
   Function    - mfCheckNewPosition
   Description - Check a move to a new location to see if its ok.
   Returns     - WadThingType
   ======================================================================== */

WadThingType const CAvatar::mfCheckNewPosition(LONG const dx, LONG const dy, LONG const Rate) const
{
	PLAYER		AvatarPlayer;
	FIXED_VECTOR_3D	newPoint;
	LONG   		Angle;
	LONG		BumpDistance;
	LONG		BumpObjectType;
	
	mfConvertPositionToPlayer(AvatarPlayer);
	newPoint.dx = dx << PLAYER_FIXEDPT;
	newPoint.dy = dy << PLAYER_FIXEDPT;
	newPoint.dz = 0;
	rot8_angle((POINT *)&newPoint, mfAngle());
	
	BumpObjectType = CheckBump(&AvatarPlayer, &newPoint, Rate);
	return (CheckMove(&AvatarPlayer, &newPoint, CHECKLINE_PLAYER, &Angle, &BumpDistance));
}
#endif // UNUSED

/* ========================================================================
   Function    - mfCheckNewPosition2
   Description - Check a move to a new location to see if its ok.
				 return wadthingtype.
   Returns     - WadThingType
   ======================================================================== */

WadThingType const CAvatar::mfCheckNewPosition2(
	FIXED_VECTOR_3D * const tp,
	LONG  const Rate) const
{
	PLAYER			AvatarPlayer;
	LONG   			Angle;
	LONG			BumpDistance;
	//LONG			BumpObjectType;
	WadThingType	BumpWadThing;
	
	mfConvertPositionToPlayer(AvatarPlayer);
	
	//BumpObjectType = CheckBump(&AvatarPlayer, tp, Rate);
	//if (BumpObjectType == iOBJECT)
	//{
	//	BumpWadThing = iOBJECT;
	//}
	//else
	//{
		BumpWadThing = CheckMove(&AvatarPlayer, tp,
				(SHORT)((TRUE == mfStayInMonsterBox())
								? CHECKLINE_MONSTER:0), &Angle, &BumpDistance);
	//}
	
	return (BumpWadThing);
}
	
/* ========================================================================
   Function    - mfCalculateAttackModifier
   Description - Subroutine used to calculate modifiers to attacks.
   Returns     - LONG modifier.
   ======================================================================== */

LONG const CAvatar::mfCalculateAttackModifier( SHORT const hEnemyTarget) const
{
	LONG Modifier = 0;
	
	if (hEnemyTarget == fERROR)
	{
		return 0;
	}
	
	CAvatar const * const pEnemyTarget = (CAvatar const * const) BLKPTR(hEnemyTarget);
	
	if (pEnemyTarget->hPlayerStats != fERROR && hPlayerStats != fERROR)
	{
		DumbAutoLockPtr<PLAYER_STATS const> const pAvatarStats(hPlayerStats);
		DumbAutoLockPtr<PLAYER_STATS const> const pEnemyStats(pEnemyTarget->hPlayerStats);
		
		if (pEnemyTarget->attrib.AttackMode == FIGHT_SEQUENCE::ATM_DEFEND &&
		    pEnemyTarget->hEnemy == hThis )
		{
			Modifier += -4;
		}
		
		if (attrib.AttackMode == FIGHT_SEQUENCE::ATM_QUICK_HIGH ||
		    attrib.AttackMode == FIGHT_SEQUENCE::ATM_QUICK_LOW)
	    {
	    	Modifier += -4;
	    }
	
	    // See whether the attack you are using is improved against this
	    // particular weak attack he is using.
	    if (attrib.AttackMode == pEnemyTarget->fFightSeq.mfGetWeakSequence(pEnemyTarget->mfType()))
	    {
	    	Modifier += 2;
	    }
	
	    // Adjust for level of difficulty in game play.
	    if (Realm.HomeRealm == ::HomeRealm)
	    {
	    	if (COMBAT_PLAY::mfGetCurrValue() == COMBAT_PLAY::EASY)
	    		Modifier += 2;
	    	else
	    	if (COMBAT_PLAY::mfGetCurrValue() == COMBAT_PLAY::HARD)
	    		Modifier -= 2;
	    }
	}
	return Modifier;
}
/* ========================================================================
   Function    - mfToHitEnemy
   Description - Get the value necessary to hit your current enemy.
   Returns     -
   ======================================================================== */

LONG const CAvatar::mfToHitEnemy(SHORT const hEnemyTarget) const
{
	LONG ToHit = 0;
	
	if (hEnemyTarget == fERROR)
	{
		return 0;
	}
	
	CAvatar * const pEnemyTarget = (CAvatar * const) BLKPTR(hEnemyTarget);
	
	if (pEnemyTarget->hPlayerStats != fERROR && hPlayerStats != fERROR)
	{
		DumbAutoLockPtr<PLAYER_STATS> const pAvatarStats(hPlayerStats);
		DumbAutoLockPtr<PLAYER_STATS> const pEnemyStats(pEnemyTarget->hPlayerStats);
		
		PLAYER_HIT const PlayerHitObj(pAvatarStats, pEnemyStats);
		
		LONG const Modifier = mfCalculateAttackModifier(hEnemyTarget);
		
		ToHit = PlayerHitObj.mfMinimumToHit(Modifier);
	}
	
	return ToHit;
}

/* ========================================================================
   Function    - mfSetMyTurnToAttack
   Description - Set my turn flag. And notify my enemy that its not their
                 turn if we are paired off.
   Returns     -
   ======================================================================== */
void CAvatar::mfSetMyTurnToAttack()
{
	if (hEnemy != fERROR)	// I have an enemy.
	{
		CAvatar * const pEnemy = (CAvatar * const) BLKPTR(hEnemy);
		
		// I am attacking and they're attacking me back.
		if (Status == AI_ATTACK && pEnemy->hEnemy == hThis)
		{
			// Make sure I'm not already in the middle of a quick attack.
			if (fAttackTurnFlag < 1)
			{
				if (mfGetAttackMode() == FIGHT_SEQUENCE::ATM_QUICK_HIGH ||
				    mfGetAttackMode() == FIGHT_SEQUENCE::ATM_QUICK_LOW)
				{
					fAttackTurnFlag = 2;
				}
				else
				{
					fAttackTurnFlag = 1;
				}
			}
		
			if (pEnemy->Status == AI_ATTACK)
			{
				// It's my turn not theirs.
				pEnemy->fAttackTurnFlag = 0;
				pEnemy->mfSetAttackSequence(FIGHT_SEQUENCE::ATM_DEFEND);
				pEnemy->mfStartAnimationOnce(pEnemy->CurSequence);
			}
			else
			{
				// They aren't attacking so just clear their flag.
				pEnemy->mfClearTurnToAttack();
			}
		}
		else
		{
			// I'm not attacking
			mfClearTurnToAttack();
			// if I'm paired off let them know they can attack at will.
			if (pEnemy->hEnemy == hThis)
			{
				pEnemy->mfClearTurnToAttack();
			}
		}
	}
	else
	{
		// I have no enemy so clear my attack flag.
		mfClearTurnToAttack();
	}
}

/* ========================================================================
   Function    - mfDecideWhoseTurnToAttack
   Description - Determin if its my turn to hit or defend. (Animations only.)
                 Then set the Turnflag between me and my enemy.
                 Called at the beginning of a fight.
   Returns     -
   ======================================================================== */
void CAvatar::mfDecideWhoseTurnToAttack()
{
	// I have an enemy and I'm attacking.
	// Its not already decided who gets to attack first.
	if (Status == AI_ATTACK &&
	    hEnemy != fERROR )
	{
	    if (fAttackTurnFlag == ATTACK_TURN_NOT_DECIDED)
	    {
			CAvatar * const pEnemy = (CAvatar * const) BLKPTR(hEnemy);
			if (pEnemy->hEnemy != hThis || pEnemy->Status != AI_ATTACK)
			{
				// If we are not paired off, I can hit any the time.
				mfSetMyTurnToAttack();
			}
			else
			{
				// Its not already their turn.
				if (pEnemy->fAttackTurnFlag == ATTACK_TURN_NOT_DECIDED)
				{
					if (random(2))	// we flip a coin.
					{
						mfSetMyTurnToAttack();
					}
					else
					{
						pEnemy->mfSetMyTurnToAttack();
					}
				}
				else
				{
					// Their already in motion, we'll just get syncronized
					if (pEnemy->fAttackTurnFlag > 0)
					{
						fAttackTurnFlag = 0;
					}
					else
					{
						mfSetMyTurnToAttack();
					}
				}
			}
		}
		// else leave the flag alone.
	}
	else
	{
		// Clear my flag as I'm not part of an attacking pair.
		mfClearTurnToAttack();
	}

}

/* ========================================================================
   Function    - mfStartNextFightSequence
   Description - If its my turn, get my next animation sequence.
   				 Called by the autoAi's to control what they do next.
   Returns     -
   ======================================================================== */

void CAvatar::mfStartNextFightSequence()
{
	if (Status == AI_ATTACK && mfGetArtSequence() == DEFENDSEQ)
	{
		if (mfIsMyTurnToAttack() && hEnemy != fERROR)
		{
			mfSetAttackSequence(mfGetAttackMode());
			mfSetMyTurnToAttack();
		}
	}
	else
	{
		mfDecrementAttackTurnCount();
		
		if (hPlayerStats != fERROR)
		{
			DumbAutoLockPtr<PLAYER_STATS const> const pPlayerStats(hPlayerStats);
			if (mfIsMyTurnToAttack() && hEnemy != fERROR)
			{
				CAvatar * const pEnemy = (CAvatar * const) BLKPTR(hEnemy);
				if (pPlayerStats->mfGetCurHitPoints() > 3  ||
				    pEnemy->hEnemy != hThis)
				{
					mfSetAttackMode(fFightSeq.mfGetNextSequence(mfType()));
				}
				else
				{
					// Try to survive but still get in some hits.
					// This is only an advantage if we are paired off.
					if (random(2) == 0)
					{
						mfSetAttackMode(fFightSeq.mfGetNextSequence(mfType()));
					}
					else
					{
						mfSetAttackMode(FIGHT_SEQUENCE::ATM_DEFEND);
					}
				}
				mfSetAttackSequence(mfGetAttackMode());
				mfSetMyTurnToAttack();
			}
			else
			{
				// Its not my turn to attack
				mfSetAttackMode(fFightSeq.mfGetNextSequence(mfType()));
				mfSetAttackSequence(FIGHT_SEQUENCE::ATM_DEFEND);
				if (hEnemy != fERROR)
				{
					CAvatar * const pEnemy = (CAvatar * const) BLKPTR(hEnemy);
					pEnemy->mfSetMyTurnToAttack();
				}
			}
			// Make an weapon swing/whoosh noise.
			mfFightStartSounds(pPlayerStats);
		}
		else
		{
			// No stats, just wack away.
			mfSetAttackMode(fFightSeq.mfGetNextSequence(mfType()));
			mfSetAttackSequence(mfGetAttackMode());
		}
	}
	mfStartAnimationOnce(CurSequence);
	
}

/* ========================================================================
   Function    - mfSethEnemy
   Description - When assigning a new enemy, call this fn with the handle.
                 (Or when removing an enemy ie fERROR)
   Returns     -
   ======================================================================== */

void CAvatar::mfSethEnemy(SHORT const EnemyHdl)
{
	if (hEnemy != fERROR)
	{
		CAvatar * pEnemy = (CAvatar *) BLKPTR(hEnemy);
		pEnemy->mfDisEngage();
		if ( mfAmIBeingControled())
		{
			pEnemy->mfTurnHighlightOff();
		}
	}
	
	if (EnemyHdl != fERROR)
	{
		CAvatar * pEnemy = (CAvatar *) BLKPTR(EnemyHdl);
		pEnemy->mfEngage();
		if ( mfAmIBeingControled())
		{
			pEnemy->mfSetHighlightEnemy();
		}
	}
	else
	{
		mfClearTurnToAttack();
	}
	hEnemy = EnemyHdl;
	mfDecideWhoseTurnToAttack();
}

/* ========================================================================
   Function    - mfIsMyTurnToAttack
   Description - Is it my turn to play the attack sequence?
   Returns     -
   ======================================================================== */

BOOL const CAvatar::mfIsMyTurnToAttack() const
{
	BOOL Result = FALSE;
	
	if (hEnemy != fERROR)
	{
		if (fAttackTurnFlag == ATTACK_TURN_NOT_DECIDED ||
		    fAttackTurnFlag >= 1)
		{
			Result = TRUE;
		}
		else if (fAttackTurnFlag == 0)
		{
			CAvatar const * const pEnemy = (CAvatar const * const) BLKPTR(hEnemy);
			if (pEnemy->hEnemy != hThis ||
			    pEnemy->Status != AI_ATTACK)
			{
				Result = TRUE;
			}
		}
	}
	
	return Result;
}
/* ========================================================================
   Function    - mfSetAttackMode
   Description - Change the mode for the avatar.
   Returns     -
   ======================================================================== */

void CAvatar::mfSetAttackMode(FIGHT_SEQUENCE::ATTACK_MODE const newMode)
{
	attrib.AttackMode = newMode;
	switch (newMode)
	{
	case FIGHT_SEQUENCE::ATM_DEFEND:
		if (Status == AI_PAUSED || Status == AI_BEGIN_PAUSE)
		{
			fPrev.Status = AI_DEFEND;
		}
		else
		{
			Status = AI_DEFEND;
		}
		break;
	case FIGHT_SEQUENCE::ATM_HIGH:
		if (Status == AI_PAUSED || Status == AI_BEGIN_PAUSE)
		{
			fPrev.Status = AI_ATTACK;
		}
		else
		{
			Status = AI_ATTACK;
		}
		break;
	case FIGHT_SEQUENCE::ATM_LOW:
		if (Status == AI_PAUSED || Status == AI_BEGIN_PAUSE)
		{
			fPrev.Status = AI_ATTACK;
		}
		else
		{
			Status = AI_ATTACK;
		}
		break;
	case FIGHT_SEQUENCE::ATM_QUICK_HIGH:
		if (Status == AI_PAUSED || Status == AI_BEGIN_PAUSE)
		{
			fPrev.Status = AI_ATTACK;
		}
		else
		{
			Status = AI_ATTACK;
		}
		break;
	case FIGHT_SEQUENCE::ATM_QUICK_LOW:
		if (Status == AI_PAUSED || Status == AI_BEGIN_PAUSE)
		{
			fPrev.Status = AI_ATTACK;
		}
		else
		{
			Status = AI_ATTACK;
		}
		break;
	case FIGHT_SEQUENCE::ATM_MAGIC:
		if (Status == AI_PAUSED || Status == AI_BEGIN_PAUSE)
		{
			fPrev.Status = AI_CASTSPELL;
		}
		else
		{
			Status = AI_CASTSPELL;
		}
		break;
	}
}

// GWP Cover fn until the engine can do this correctly.
BOOL const CAvatar::mfTestSequenceDone() const
{
	BOOL Result = FALSE;
	
	switch(CurSequence)
	{
	case STANDSEQ:		// 1 frame
	case LISTENSEQ:
	case DEADSEQ:
		if (fFrameLastChanged < SCENE_MGR::gFrame)
			Result = TRUE;
		break;
	case DEFENDSEQ:		// 5 frames
	case GEST1SEQ:
	case BOWSEQ:
	case SITGESTSEQ:
		if ((4 + fFrameLastChanged) < SCENE_MGR::gFrame)
			Result = TRUE;
		break;
	case ATTACK1SEQ:	// 9 frames
	case ATTACK2SEQ:
		if (mfIsQuickAnimation())
		{
			if ((4 + fFrameLastChanged) < SCENE_MGR::gFrame)
				Result = TRUE;
		}
		else
		{
			if (mfType() == FIREBALL_1 ||		// Fireballs are 18 frames.
			    mfType() == LORES_FIREBALL_1)
			{
				if ((17 + fFrameLastChanged) < SCENE_MGR::gFrame)
					Result = TRUE;
			}
			else if (mfType() == PLASMA_BALL_1 ||	// only have 1 frame.
			         mfType() == LORES_PLASMA_BALL_1 ||
			         mfType() == PLASMA_BALL_2 ||
			         mfType() == LORES_PLASMA_BALL_2 ||
			         mfType() == PLASMA_STREAK_1 ||
			         mfType() == LORES_PLASMA_STREAK_1 ||
			         mfType() == LIGHTNING_1 ||
			         mfType() == LORES_LIGHTNING_1)
			{
				if ((fFrameLastChanged) < SCENE_MGR::gFrame)
					Result = TRUE;
			}
			else
			{
				if ((8 + fFrameLastChanged) < SCENE_MGR::gFrame)
					Result = TRUE;
			}
		}
		break;
	case EXPIRESEQ:
		if (mfType() == PLASMA_BALL_1 ||	// have 18 frames.
		         mfType() == LORES_PLASMA_BALL_1 ||
		         mfType() == PLASMA_BALL_2 ||
		         mfType() == LORES_PLASMA_BALL_2 ||
		         mfType() == PLASMA_STREAK_1 ||
		         mfType() == LORES_PLASMA_STREAK_1 ||
		         mfType() == LIGHTNING_1 ||
		         mfType() == LORES_LIGHTNING_1)
		{
			if ((17 + fFrameLastChanged) < SCENE_MGR::gFrame)
				Result = TRUE;
		}
		else
		{
			if ((8 + fFrameLastChanged) < SCENE_MGR::gFrame)
				Result = TRUE;
		}
		break;
	case CROUCHSEQ:
	case MARCHSEQ:
	case WALKSEQ:
		if ((8 + fFrameLastChanged) < SCENE_MGR::gFrame)
			Result = TRUE;
		break;
	}
	
	// GWP When the engine does this correctly do the following instead
	// return (TestSequenceDone(CurSequence);
	return Result;
}

// GWP Cover fn until the engine can do this correctly.
BOOL const CAvatar::mfTestSequencePastHalf() const
{
	BOOL Result = FALSE;
	
	switch(CurSequence)
	{
	case STANDSEQ:		// 1 frame
	case LISTENSEQ:
	case DEADSEQ:
			Result = TRUE;
		break;
	case DEFENDSEQ:		// 5 frames
	case GEST1SEQ:
	case BOWSEQ:
	case SITGESTSEQ:
		if ((2 + fFrameLastChanged) < SCENE_MGR::gFrame)
			Result = TRUE;
		break;
	case ATTACK1SEQ:	// 9 frames
	case ATTACK2SEQ:
		if (mfIsQuickAnimation())
		{
			if ((2 + fFrameLastChanged) < SCENE_MGR::gFrame)
				Result = TRUE;
		}
		else
		{
			if (mfType() == FIREBALL_1 ||		// Fireballs are 18 frames.
			    mfType() == LORES_FIREBALL_1)
			{
				if ((8 + fFrameLastChanged) < SCENE_MGR::gFrame)
					Result = TRUE;
			}
			else if (mfType() == PLASMA_BALL_1 ||	// only have 1 frame.
			         mfType() == LORES_PLASMA_BALL_1 ||
			         mfType() == PLASMA_BALL_2 ||
			         mfType() == LORES_PLASMA_BALL_2 ||
			         mfType() == PLASMA_STREAK_1 ||
			         mfType() == LORES_PLASMA_STREAK_1 ||
			         mfType() == LIGHTNING_1 ||
			         mfType() == LORES_LIGHTNING_1)
			{
				if ((fFrameLastChanged) < SCENE_MGR::gFrame)
					Result = TRUE;
			}
			else
			{
				if ((4 + fFrameLastChanged) < SCENE_MGR::gFrame)
					Result = TRUE;
			}
		}
		break;
	case EXPIRESEQ:
		if (mfType() == PLASMA_BALL_1 ||	// have 18 frames.
		         mfType() == LORES_PLASMA_BALL_1 ||
		         mfType() == PLASMA_BALL_2 ||
		         mfType() == LORES_PLASMA_BALL_2 ||
		         mfType() == PLASMA_STREAK_1 ||
		         mfType() == LORES_PLASMA_STREAK_1 ||
		         mfType() == LIGHTNING_1 ||
		         mfType() == LORES_LIGHTNING_1)
		{
			if ((8 + fFrameLastChanged) < SCENE_MGR::gFrame)
				Result = TRUE;
		}
		else
		{
			if ((4 + fFrameLastChanged) < SCENE_MGR::gFrame)
				Result = TRUE;
		}
		break;
	case CROUCHSEQ:
	case MARCHSEQ:
	case WALKSEQ:
		if ((4 + fFrameLastChanged) < SCENE_MGR::gFrame)
			Result = TRUE;
		break;
	}
	
	// GWP When the engine does this correctly do the following instead
	// return (TestSequenceDone(CurSequence);
	return Result;
}
// GWP Cover fn until the engine can do this correctly.
BOOL const CAvatar::mfTestSequenceAtHit() const
{
	BOOL Result = FALSE;
	
	switch(CurSequence)
	{
	case ATTACK1SEQ:	// 9 frames
	case ATTACK2SEQ:
		if (mfIsQuickAnimation())
		{
			if ((2 + fFrameLastChanged) < SCENE_MGR::gFrame &&
			    (4 + fFrameLastChanged) > SCENE_MGR::gFrame
			   )
				Result = TRUE;
		}
		else
		{
			if (mfType() == FIREBALL_1 ||		// Fireballs are 18 frames.
			    mfType() == LORES_FIREBALL_1)
			{
				if ((8 + fFrameLastChanged) < SCENE_MGR::gFrame &&
				    (10 + fFrameLastChanged) > SCENE_MGR::gFrame
				   )
					Result = TRUE;
			}
			else if (mfType() == PLASMA_BALL_1 ||	// only have 1 frame.
			         mfType() == LORES_PLASMA_BALL_1 ||
			         mfType() == PLASMA_BALL_2 ||
			         mfType() == LORES_PLASMA_BALL_2 ||
			         mfType() == PLASMA_STREAK_1 ||
			         mfType() == LORES_PLASMA_STREAK_1 ||
			         mfType() == LIGHTNING_1 ||
			         mfType() == LORES_LIGHTNING_1)
			{
				if ((fFrameLastChanged) < SCENE_MGR::gFrame)
					Result = TRUE;
			}
			else
			{
				if ((4 + fFrameLastChanged) < SCENE_MGR::gFrame &&
				    (6 + fFrameLastChanged) > SCENE_MGR::gFrame
				   )
					Result = TRUE;
			}
		}
		break;
	}
	
	return Result;
}

// Test for nearly done for Magic animations.
BOOL const CAvatar::mfTestSequence2ndLastFrame() const
{
	BOOL Result = FALSE;
	
	switch(CurSequence)
	{
	case STANDSEQ:		// 1 frame
	case LISTENSEQ:
	case DEADSEQ:
			Result = TRUE;
		break;
	case DEFENDSEQ:		// 5 frames
	case GEST1SEQ:
	case BOWSEQ:
	case SITGESTSEQ:
		if ((3 + fFrameLastChanged) < SCENE_MGR::gFrame)
			Result = TRUE;
		break;
	case ATTACK1SEQ:	// 9 frames
	case ATTACK2SEQ:
		if (mfIsQuickAnimation())
		{
			if ((3 + fFrameLastChanged) < SCENE_MGR::gFrame)
				Result = TRUE;
		}
		else
		{
			if (mfType() == FIREBALL_1 ||		// Fireballs are 18 frames.
			    mfType() == LORES_FIREBALL_1)
			{
				if ((16 + fFrameLastChanged) < SCENE_MGR::gFrame)
					Result = TRUE;
			}
			else if (mfType() == PLASMA_BALL_1 ||	// only have 1 frame.
			         mfType() == LORES_PLASMA_BALL_1 ||
			         mfType() == PLASMA_BALL_2 ||
			         mfType() == LORES_PLASMA_BALL_2 ||
			         mfType() == PLASMA_STREAK_1 ||
			         mfType() == LORES_PLASMA_STREAK_1 ||
			         mfType() == LIGHTNING_1 ||
			         mfType() == LORES_LIGHTNING_1)
			{
				if ((fFrameLastChanged) < SCENE_MGR::gFrame)
					Result = TRUE;
			}
			else
			{
				if ((7 + fFrameLastChanged) < SCENE_MGR::gFrame)
					Result = TRUE;
			}
		}
		break;
	case EXPIRESEQ:
		if (mfType() == PLASMA_BALL_1 ||	// have 18 frames.
		         mfType() == LORES_PLASMA_BALL_1 ||
		         mfType() == PLASMA_BALL_2 ||
		         mfType() == LORES_PLASMA_BALL_2 ||
		         mfType() == PLASMA_STREAK_1 ||
		         mfType() == LORES_PLASMA_STREAK_1 ||
		         mfType() == LIGHTNING_1 ||
		         mfType() == LORES_LIGHTNING_1)
		{
			if ((16 + fFrameLastChanged) < SCENE_MGR::gFrame)
				Result = TRUE;
		}
		else
		{
			if ((7 + fFrameLastChanged) < SCENE_MGR::gFrame)
				Result = TRUE;
		}
		break;
	case CROUCHSEQ:
	case MARCHSEQ:
	case WALKSEQ:
		if ((7 + fFrameLastChanged) < SCENE_MGR::gFrame)
			Result = TRUE;
		break;
	}
	return Result;
}

// [d5-23-97 JPC] Made the following functions non-inline:
//		mfSetDamageFlag
//		mfTestAndGiveDamage
//		mfTestAndGiveMagicDamage
//
// also checked for fireball in mfSetDamageFlag.

// Check attack mode and set the Damage code appropriately.
void CAvatar::mfSetDamageFlag()
{
	// [d5-23-97 JPC] If the avatar uses fireball AI (fireballs,
	// magic missiles, disintegrate, chain lightning, etc.),
	// then its fDamage member should NOT be modified; just return.
	if (AIFuncIndex == AI_FUNC_FIREBALL)
	{
// #if defined (_DEBUG)
// 		printf ("Not applicable to fireball\n");
// #endif
		return;
	}

	SBYTE NumberOfAttacks = 1;
	fDamage = -1; // No damage rolled yet.
	
	if (hPlayerStats != fERROR)
	{
		PLAYER_STATS const * const pPlayerStats = (PLAYER_STATS const * const) BLKPTR(hPlayerStats);
		NumberOfAttacks = pPlayerStats->mfGetNumberOfAttacks();
	}
	
	switch(attrib.AttackMode)
	{
	case FIGHT_SEQUENCE::ATM_QUICK_HIGH:
	case FIGHT_SEQUENCE::ATM_QUICK_LOW:
		fDamageFlag = 2 * NumberOfAttacks;
		break;
	case FIGHT_SEQUENCE::ATM_HIGH:
	case FIGHT_SEQUENCE::ATM_LOW:
		fDamageFlag = 1 * NumberOfAttacks;
		break;
	case FIGHT_SEQUENCE::ATM_MAGIC:
		fDamageFlag = 1;
		break;
	case FIGHT_SEQUENCE::ATM_NONE:
		// This avatar doesn't use the fight stuff. (Wyverns.)
		if (Status == AI_ATTACK ||
		    (Status == AI_PAUSED && fPrev.Status == AI_ATTACK))
		{
			fDamageFlag = 1 * NumberOfAttacks;
		}
		break;
	}
}

void CAvatar::mfTestAndGiveDamage(HDL_AVATAR const hEnemyTarget)
{
	if (CurSequence != DEFENDSEQ && mfTestSequenceAtHit())
	{
		if (!mfIsAllDamageDone())
		{
			mfWackEm(hEnemyTarget);
			attrib.PlayedFightSnd = TRUE;
		}
		else
		{
			if (!attrib.PlayedFightSnd)
			{
				mfFightSounds(hEnemyTarget);
				attrib.PlayedFightSnd = TRUE;
			}
		}
	}
}

void CAvatar::mfTestAndGiveMagicDamage(HDL_AVATAR const hEnemyTarget,
                              THINGTYPE const ThingCausingDamage,
                              LONG const Damage)
{
	if (mfTestSequence2ndLastFrame())
	{
		if (!mfIsAllDamageDone())
		{
			if (hEnemyTarget != fERROR)
			{
				CAvatar * const pEnemy = (CAvatar * const) BLKPTR(hEnemyTarget);
				mfInflictMagicDamage(pEnemy, ThingCausingDamage, Damage);
			}
		}
		else
		{
			if (!attrib.PlayedFightSnd)
			{
				attrib.PlayedFightSnd = TRUE;
			}
		}
	}
}

/* ========================================================================
   Function    - mfIsHomeTeam
	Description - returns TRUE if on the home team
	Returns     - 
	======================================================================== */
BOOL const CAvatar::mfIsHomeTeam() const
{
	//Use the scene manager instead of the units info.
	//GEH wrong question
	// return (SCENE_MGR::Visitors == Realm.HomeRealm);
	
	return (units[SCENE_MGR::HomeIndex].Realm == Realm.HomeRealm);
	
}


