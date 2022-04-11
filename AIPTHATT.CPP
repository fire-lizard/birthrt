
/* ========================================================================
   Copyright (c) 1990,1996	  Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aipthatt.cpp
   Author:   Greg Hightower & Gary Powell
   ======================================================================== */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "system.h"
#include "engine.h"

#include "ai_utils.h"
#include "avatar.hxx"
#include "scnmgr.hxx"


/* ========================================================================
   Function	- mfCreateFollowers
   Description - Make some troops 
   Returns	 - void
   ======================================================================== */
#if 0 // UNUSED
void PATH_ATTACK_DATA::mfCreateFollowers(CAvatar *pAvatar)
{
	LONG i;
	// Note : Don't lock the scene here, its already locked.
	PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	const LONG X = pAvatar->mfX();
	const LONG Y = pAvatar->mfY();
	const LONG Z = pAvatar->mfZ();
	LONG IdStart = pAvatar->Id*25+1000; // make groups of 25
	SHORT		*pList;
	PLAYER	*pPath;
	
	// init leader data
	Leader.sCount = 0;
	Leader.sMaxFollowers = 2;
	
	Leader.hdlSlotList = NewBlock(sizeof(SHORT) * Leader.sMaxFollowers);
	pList = (SHORT *)BLKPTR(Leader.hdlSlotList);
	
	// Initialize the List to no followers (in case we blow up below.)
	for (i = 0; i < Leader.sMaxFollowers; i++)
	{
		pList[i] = fERROR;
	}
	
	Leader.hdlPathDeltas = NewBlock(sizeof(PLAYER) * Leader.sMaxFollowers);
	pPath = (PLAYER *)BLKPTR(Leader.hdlPathDeltas);
	
	// create my troops and record their handles in my follower list
	for (i=0; i< Leader.sMaxFollowers; i++)
	{
		LONG index = CAvatar::CreateAvatar( IdStart, 
											pAvatar->mfType(), 
											X, Y, 0, 
											CAvatar::AI_FUNC_FOLLOW_ID , 
											pCurrentScene, 
											pAvatar->Realm.HomeRealm);
		
		if (index != fERROR )
		{
			CAvatar *pTroop = (CAvatar *) BLKPTR(pCurrentScene->Avatars[index]);
			pTroop->attrib.RuntimeCreated = TRUE;
			
			pTroop->fFollowId.Id = pAvatar->Id;	// who to follow
			// Make them from the same realm as the leader.
			pTroop->fFollowId.Rate = 2 * pAvatar->mfGetMarchRate();			// run a little faster then my captain

			pList[i] = AvatarIdToHdl(pTroop->Id);
			
			IdStart++;
		}
		else
		{
			// No slots left.
			printf("WARNING! Unable to create requested troops. %d\n", (8 - i));
			return;
		}
		
		pPath[i].x = pAvatar->mfX();
		pPath[i].y = pAvatar->mfY() + (i*OffSet);
	}
}
#endif
/* ========================================================================
   Function	- PathAttack
   Description - AI function to follow a point path from a file on disk
   Returns	 - Current state
   ======================================================================== */

#if 0 // UNUSED
void CAvatar::PathAttack (CAvatar *pAvatar, CAvatar::AISTATUS Status)
{
	switch( Status )
	{
	case AI_INIT:
		if (pAvatar->fPathAttack.FollowPath.PathCount > 0)
		{
			LONG X;
			LONG Y;
			LONG Z;
			LONG A;
			
			pAvatar->fPathAttack.FollowPath.mfGetCurrentPt(X, Y, Z, A);
			
			pAvatar->Status = AI_MOVING;
			//pAvatar->Rate = pAvatar->fPathAttack.TrailRate;

			pAvatar->mfMoveToXYZA( X, Y, Z, A );
			
			pAvatar->mfStartAnimationLoop( WALKSEQ);
		}
		else
		{
			pAvatar->mfStartAnimationLoop( STANDSEQ);
		}
		pAvatar->fPathAttack.mfCreateFollowers(pAvatar);
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
		
		pAvatar->fPathAttack.FollowPath.mfGetCurrentPt(X, Y, Z, A);
		
		if( ( pAvatar->mfX() == X ) &&
			( pAvatar->mfY() == Y ) )
		{
			// if I'm at end of path, attack
			if(!pAvatar->fPathAttack.FollowPath.mfGetNextPt(X, Y, Z, A))
				pAvatar->Status = AI_ATTACK;
		}
		else
		{
			// record where I've been
			pAvatar->fPathAttack.Leader.mfRecordTrail(pAvatar);
		
			// if not at destination, start moving in that direction
			LONG dx = (X - pAvatar->mfX());
			LONG dy = (Y - pAvatar->mfY());
			
			// move toward my target
			const LONG h = dist( pAvatar->mfX(), pAvatar->mfY(), X, Y );
			
			if( abs(dx) > pAvatar->fPathAttack.TrailRate )
			{
				dx = dx * pAvatar->fPathAttack.TrailRate / h;
			}
			
			if( abs(dy) > pAvatar->fPathAttack.TrailRate )
			{
				dy = dy * pAvatar->fPathAttack.TrailRate / h;
			}
			
			pAvatar->FaceTo(pAvatar->mfX() + dx, 
						   pAvatar->mfY() + dy );
						   
			pAvatar->mfMoveToXYZA( pAvatar->mfX() + dx, 
							  pAvatar->mfY() + dy, Z, pAvatar->mfAngle());
							  
		}
			
		// now update anyone following me
		pAvatar->fPathAttack.Leader.mfMoveFollowers(pAvatar);
		
		}
		break;
			
	case AI_ATTACK:
		{
		SHORT	rndnum;
		rndnum = random(6);
		if(rndnum == 0)
			pAvatar->mfStartAnimationOnce(ATTACK1SEQ);
		else
		if(rndnum == 1)
			pAvatar->mfStartAnimationOnce(ATTACK2SEQ);
		else
		if(rndnum == 2)
			pAvatar->mfStartAnimationOnce(DEFENDSEQ);
		else
			pAvatar->mfStartAnimationOnce(STANDSEQ);
		}
		// now update anyone following me
		//pAvatar->fPathAttack.Leader.mfMoveFollowers(pAvatar);
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
#endif

