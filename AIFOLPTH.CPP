/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aifolpth.cpp
   Author:   Greg Hightower
   ======================================================================== */

/* -----------------------------------------------------------------
   additional includes
   ----------------------------------------------------------------- */

#include "AIFOLPTH.HXX"
#include "FILEUTIL.H"
#include "AVATAR.HXX"

/* -----------------------------------------------------------------
   class definition
   ----------------------------------------------------------------- */


/* -----------------------------------------------------------------
   inline member functions
   ----------------------------------------------------------------- */

void FOLLOW_PATH_DATA::mfWriteData(FILE * fp)
{
	fprintf(fp, "%ld\n", PathCount);
	if (PathCount > 0)
	{
		LONG i;
		PTR_PATH_PTS pPathPt = (PTR_PATH_PTS) BLKPTR(hPath);
		
		for (i = 0 ; i < PathCount; i++)
		{
			pPathPt[i].mfWriteData(fp);
		}
	}
}

LONG FOLLOW_PATH_DATA::mfReadData(FILE *fp)
{
	char cpBuffer[80];
	LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	
	PathIndex = 0;
	oldP.x = 0;
	oldP.y = 0;
	if (1 == sscanf(cpBuffer, "%ld", &PathCount))
	{
		if (PathCount > 0)
		{
			hPath = NewBlock(sizeof(PATH_PT) * PathCount);
			
			if (hPath != fERROR)
			{
				SHORT i;
				
				SetBlockAttr(hPath,LOCKED, LOCKED);
				PTR_PATH_PTS pPathPts = (PTR_PATH_PTS) BLKPTR(hPath);
				
				for (i = 0; i < PathCount && Result != EOF; i++)
				{
					Result = pPathPts[i].mfReadData(fp);
				}
				
				if (i < PathCount)
				{
					// Premature end of file. Lets at least use bad data.
					PathCount = i;
				}
				ClrLock(hPath);
			}
			else
			{
				PathCount = 0;
				hPath = fERROR;
				Result = EOF; // Hey we're nearly out of memory anyway.
			}
		}
	}
	else
	{
		PathCount = 0;
		hPath = fERROR;
		Result = EOF;
	}
	return Result;
}


BOOL FOLLOW_PATH_DATA::mfGetNextPt(LONG &X, LONG &Y, LONG &Z, LONG &A)
{
	BOOL retVal = TRUE;
	if (hPath != fERROR)
	{
		PathIndex++;
		if (PathIndex >= PathCount)
		{
			PathIndex = 0;
			retVal = FALSE;
		}
		PTR_PATH_PTS pPathPts = (PTR_PATH_PTS) BLKPTR(hPath);
		X = pPathPts[PathIndex].X;
		Y = pPathPts[PathIndex].Y;
		Z = pPathPts[PathIndex].Z;
		A = pPathPts[PathIndex].A;
	}
	else
	{
		retVal = FALSE;
	}
	return retVal;
}
/* ========================================================================
   Function	- FollowPath
   Description - AI function to follow a point path from a file on disk
   Returns	 - Current state
   ======================================================================== */

void CAvatar::FollowPath (CAvatar *pAvatar, CAvatar::AISTATUS Status)
{
	pAvatar->attrib.IsFollowPath = TRUE;
	switch( Status )
	{
	case AI_INIT:
			pAvatar->fFollowPath.Wave = 0;
			if (pAvatar->fFollowPath.PathCount > 0)
			{
				//LONG X;
				//LONG Y;
				//LONG Z;
				//LONG A;
				
				//pAvatar->fFollowPath.mfGetCurrentPt(X, Y, Z, A);
				
				pAvatar->Status = AI_MOVING;

				if(pAvatar->mfType() ==  WYVERN_1 ||
				   pAvatar->mfType() == T_WYVERN_1 ||
				   pAvatar->mfType() == WYVERN_2  ||
				   pAvatar->mfType() == T_WYVERN_2)
				{
					pAvatar->mfStartAnimationLoop(MARCHSEQ);
				}
				else
				{
					pAvatar->mfStartAnimationLoop( WALKSEQ);
				}
			}
			else
			{
				pAvatar->mfStartAnimationLoop( STANDSEQ);
			}
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
			PLAYER TempPlayer;
			LONG Rate;
			FIXED_VECTOR_3D NewVector;
			WadThingType bump;
			LONG	tmpA;
			LONG	distance;
			SHORT   count=0;
			LONG	BumpDistance;
			if(pAvatar->mfType() == WYVERN_1||
			   pAvatar->mfType() == T_WYVERN_1 ||
			   pAvatar->mfType() == T_WYVERN_2 ||
			   pAvatar->mfType() == WYVERN_2)
			{
				Rate = pAvatar->mfGetMarchRate();
			}
			else
			{
				Rate = pAvatar->mfGetWalkRate();
			}
			
			pAvatar->hEnemy = pAvatar->mfFindClosestFoe();
			
			if (pAvatar->hEnemy != fERROR)
			{
			 	CAvatar * pAvatarTarget = (CAvatar *) BLKPTR(pAvatar->hEnemy);
				distance = aprox_dist(
							pAvatar->mfX(), pAvatar->mfY(),
							pAvatarTarget->mfX(), pAvatarTarget->mfY());
	
			 	if ( pAvatar->mfCanSeeAvatar(pAvatarTarget)
					&&distance <= CAvatar::ActivationDistance)
			   	{
				   pAvatar->fFollowPath.oldP.x = pAvatar->mfX();
				   pAvatar->fFollowPath.oldP.y = pAvatar->mfY();	
				   switch (pAvatar->mfType())
				   {
				   case GIANT_SPIDER:
				   case T_GIANT_SPIDER:
				   case CEILING_SPIDER:
				   case T_CEILING_SPIDER:
				   	pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_SPIDERS);
				   	break;
				   case HARPY:
				   case T_HARPY:
				   	pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_HARPIES);
				   	break;
				   case WYVERN_1:
				   case T_WYVERN_1:
				   case WYVERN_2:
				   case T_WYVERN_2:
				   	pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_WYVERNS);
				   	break;
				   case DOG:
				   case T_DOG:
				   case HELL_HOUND:
				   case T_HELL_HOUND:
				   	pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_HELLHOUND);
				   	break;
				   default:
				   	pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_PING_PONG);
				   	break;
				   }
				   
				   pAvatar->mfInitVals();
				   pAvatar->DoAI(CAvatar::AI_INIT);
				   break;
				}
				else 
					goto Continue_Follow_Path;
			}
			else
			{
Continue_Follow_Path:
				pAvatar->fFollowPath.mfGetCurrentPt(X, Y, Z, A);
				
				if( ( pAvatar->mfX() == X ) &&
					( pAvatar->mfY() == Y ) )
				{
						// You've made it to the new location, get the next
						// requested destination.
						pAvatar->fFollowPath.mfGetNextPt(X, Y, Z, A);
						pAvatar->fFollowPath.oldP.x = 0;
						pAvatar->fFollowPath.oldP.y = 0;
				}
				
				pAvatar->mfConvertPositionToPlayer(TempPlayer);
				if(pAvatar->fFollowPath.oldP.x != 0 || pAvatar->fFollowPath.oldP.y != 0)
				{
					distance = aprox_dist(pAvatar->mfX(), pAvatar->mfY(), X, Y);
					tmpA = AngleFromPoint2(pAvatar->mfX(), pAvatar->mfY(), X, Y, 0);
					bump=CheckLongMove(&TempPlayer, tmpA, distance, CHECKLINE_MONSTER, Z, &BumpDistance);
					while (bump!= iNOTHING && count >= pAvatar->fFollowPath.PathCount)
					{
						pAvatar->fFollowPath.mfGetNextPt(X, Y, Z, A);
						distance = aprox_dist(pAvatar->mfX(), pAvatar->mfY(), X, Y);
						tmpA = AngleFromPoint2(pAvatar->mfX(), pAvatar->mfY(), X, Y, 0);
						bump=CheckLongMove(&TempPlayer, tmpA, distance, CHECKLINE_MONSTER, Z, &BumpDistance);
						count++;
					}
					if(count >= pAvatar->fFollowPath.PathCount)   // couldn't get back to the path, change to pong
					{
					   switch (pAvatar->mfType())
					   {
					   case GIANT_SPIDER:
					   case T_GIANT_SPIDER:
					   case CEILING_SPIDER:
					   case T_CEILING_SPIDER:
					   	pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_SPIDERS);
					   	break;
					   case HARPY:
					   case T_HARPY:
					   	pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_HARPIES);
					   	break;
					   case WYVERN_1:
					   case T_WYVERN_1:
					   case WYVERN_2:
					   case T_WYVERN_2:
					   	pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_WYVERNS);
					   	break;
					   case DOG:
					   case T_DOG:
					   case HELL_HOUND:
					   case T_HELL_HOUND:
					   	pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_HELLHOUND);
					   	break;
					   default:
					   	pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_PING_PONG);
					   	break;
					   }
					   
					   pAvatar->mfInitVals();
					   pAvatar->DoAI(CAvatar::AI_INIT);
					   pAvatar->attrib.IsFollowPath = FALSE;
					   break;
					}
					pAvatar->FaceTo(X, Y);



				}
				const LONG dx = X - pAvatar->mfX();
				const LONG dy = Y - pAvatar->mfY();
				const LONG dz = Z - pAvatar->mfZ();
				distance = aprox_dist(
							pAvatar->mfX(), pAvatar->mfY(),
							X, Y);
				
				if (ABS(distance) < Rate)
				{
					NewVector.dx = dx;
					NewVector.dy = dy;
					NewVector.dz = dz;
				}
				else
				{
					NewVector.dx = ((dx * Rate)/distance);
					NewVector.dy = ((dy * Rate)/distance);
					NewVector.dz = ((dz * Rate)/distance);
				}
				NewVector.dx <<= PLAYER_FIXEDPT;
				NewVector.dy <<= PLAYER_FIXEDPT;
			
				//if (pAvatar->mfIsVisible())
				//{
					CheckBump(&TempPlayer, &NewVector, pAvatar->mfGetWalkRate());
				//}

				if(pAvatar->fFollowPath.oldP.x != 0 || pAvatar->fFollowPath.oldP.y != 0)
					pAvatar->mfMoveToXYZA(
						pAvatar->mfX() + PLAYER_INT_VAL(NewVector.dx),
						pAvatar->mfY() + PLAYER_INT_VAL(NewVector.dy),
						pAvatar->mfZ() + NewVector.dz,
						pAvatar->mfAngle());
				else
					pAvatar->mfMoveToXYZA(
						 pAvatar->mfX() + PLAYER_INT_VAL(NewVector.dx),
						 pAvatar->mfY() + PLAYER_INT_VAL(NewVector.dy),
						 pAvatar->mfZ() + NewVector.dz,
						 A);
		
				
				}
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

