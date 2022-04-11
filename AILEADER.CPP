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
#include "system.h"
#include "engine.h"

#include "ai_utils.h"
#include "avatar.hxx"

static const SHORT sFrameCountDelay = 6;

/* ========================================================================
   Function    - mfWriteData
   Description - Write to Scene File
   Returns     - 
   ======================================================================== */
void LEADER_DATA::mfWriteData(FILE *fp) 
{
	SHORT		sActualFollowers = 0;
	SHORT		i;
	
	SHORT *pShortList = (SHORT *)(BLKPTR(hdlSlotList));
	for(i=0;i<sMaxFollowers;i++)
	{
		if (pShortList[i] != (SHORT)fERROR)
			sActualFollowers++;
	}
	fprintf(fp, "%hd %hd\n", sActualFollowers, sMaxFollowers);
	for(i=0;i<sMaxFollowers;i++)
	{
		if (pShortList[i] != (SHORT)fERROR)
			fprintf(fp, "%hd\n", AvatarHdlToId(pShortList[i]));
	}
	fprintf(fp, "\n");
	
	PLAYER *pPointList = (PLAYER *)(BLKPTR(hdlPathDeltas));
	if (sActualFollowers > 0)
	{
		for(i=0;i<sMaxFollowers;i++)
		{
			fprintf(fp, "%ld %ld %ld %ld %ld\n", 
				pPointList[i].x, 
				pPointList[i].y, 
				pPointList[i].z, 
				pPointList[i].a,
				pPointList[i].p);
		}
	}
}
	
/* ========================================================================
   Function    - mfReadData
   Description - Read from the Scene File
   Returns     - 
   ======================================================================== */
LONG LEADER_DATA::mfReadData(FILE * fp)
{
	char cpBuffer[128];
	LONG Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	LONG		i;
	SHORT		sActualFollowers;
	SHORT		*pShortList;
	PLAYER		*pPointList;
	
	Result = (2 == sscanf(cpBuffer, "%hd %hd", &sActualFollowers, &sMaxFollowers) ) ? Result : EOF;
	if (Result != EOF && sMaxFollowers != 0)
	{
		if (sActualFollowers > 0)
		{
			hdlSlotList = NewBlock(sizeof(SHORT) * sMaxFollowers);
			if (hdlSlotList == fERROR)
				return EOF;
			pShortList = (SHORT *)(BLKPTR(hdlSlotList));
			// read in the id of who's following me
			// INIT will convert Ids to Handles
			for(i=0;i<sActualFollowers;i++)
			{
				Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
				if (Result == EOF)
					return EOF;
				sscanf(cpBuffer, "%hd", &pShortList[i]);
			}
			// clear out rest of the list
			for(i=sActualFollowers;i<sMaxFollowers;i++)
			{
				pShortList[i] = fERROR;
			}
		}
		
		if (sActualFollowers > 0)
		{
			hdlPathDeltas = NewBlock(sizeof(PLAYER) * sMaxFollowers);
			if (hdlPathDeltas == fERROR)
				return EOF;
			pPointList = (PLAYER *)(BLKPTR(hdlPathDeltas));
			for(i=sActualFollowers- 1;i>=0;i--)
			{
				Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
				if (Result == EOF)
					return EOF;
				if(3 != sscanf(cpBuffer, "%ld %ld %ld", 
					&pPointList[i].x,&pPointList[i].y,&pPointList[i].a))
					return EOF;
				
			}
		}
		sHead = sActualFollowers;
	}
	
	return Result;
}

/* ========================================================================
   Function    - fLeader::mfInit
   Description - init the leader data
   Returns     - 
   ======================================================================== */
void LEADER_DATA::mfInit(CAvatar *pAvatar)
{
	LONG		i;
	SHORT		*pList;
	PLAYER	*pPath;
	
	// Could have been allocated by reading the scene file
	if (hdlPathDeltas == fERROR)
	{
		hdlPathDeltas = NewBlock(sizeof(PLAYER) * sMaxFollowers);
		if (hdlPathDeltas == fERROR)
			goto LeaveFn;
		
		pPath = (PLAYER *)BLKPTR(hdlPathDeltas);
		for (i= 0; i< sMaxFollowers; i++)
		{
			pPath[i].x = 0;
			pPath[i].y = -i * (pAvatar->mfWidth() * 3); // GWP some werid magic numbers that may work.
		}
	}
	else
	{
		pPath = (PLAYER *)BLKPTR(hdlPathDeltas);
	}
	
	// Could have been allocated by reading the scene file
	if (hdlSlotList == fERROR)
	{
		hdlSlotList = NewBlock(sizeof(SHORT) * sMaxFollowers);
		if (hdlSlotList == fERROR)
			goto LeaveFn;
	
		pList = (SHORT *)BLKPTR(hdlSlotList);
		for (i = 0; i < sMaxFollowers; i++)
		{
			pList[i] = fERROR;
		}
	}
	else
	{
		pList = (SHORT *)BLKPTR(hdlSlotList);
	}
	
	// preset the list of locations
	// and
	// convert the Id's to actual handles
	
	for (i=0;i < sMaxFollowers; i++)
	{
		// convert Id's to Avatar Handles
		if (pList[i] != fERROR)
		{
			pList[i] = AvatarIdToHdl(pList[i]);
		}
		
		// convert delta offsets to real coords
		// GWP I don't figure we should call CheckBump and CheckMove here,
		// GWP but maybe we do have to.
		pPath[i].x += pAvatar->mfX();
		pPath[i].y += pAvatar->mfY();
		
	}

LeaveFn:
	sCount = 0;
}

/* ========================================================================
   Function    - CAvatar::fLeader::mfRecordTrail
   Description - update trail info 
   Returns     - 
   ======================================================================== */
void LEADER_DATA::mfRecordTrail(CAvatar *pAvatar)
{
	PLAYER	*pPath;
	
	if (hdlPathDeltas != fERROR)
	{
		sCount++;
		if(sCount >= sFrameCountDelay)
		{
			sCount = 0;
			// retrieve the path array
			pPath = (PLAYER *)BLKPTR(hdlPathDeltas);
			// save the data into the current head pointer
			pPath[sHead].x = pAvatar->mfX();
			pPath[sHead].y = pAvatar->mfY();
			pPath[sHead].a = pAvatar->mfAngle();
			// wrap the head pointer if at end of array
			sHead = (SHORT) ((sHead < sMaxFollowers- 1) ? sHead+1 : 0 );
		}
	}
}
	
/* ========================================================================
   Function    - CAvatar::fLeader::mfMoveFollowers
   Description - move followers
   Returns     - 
   ======================================================================== */
void LEADER_DATA::mfMoveFollowers(CAvatar *) // pAvatar (pointer to yourself. 
{
	PLAYER	*pPath;
	SHORT		*pList;
	SHORT		i;
	
	if (hdlSlotList != fERROR)
	{
		SHORT	sIndex;
		// retrieve the path array
		pPath = (PLAYER *)BLKPTR(hdlPathDeltas);
		pList = (SHORT *)BLKPTR(hdlSlotList);
	
		for (i=0;i < sMaxFollowers; i++)
		{
			sIndex = (SHORT)(sHead - i -  1);
			if (sIndex < 0)
				sIndex += sMaxFollowers;
				
			// first try to compact the list
			// if (pList[i] == fERROR)
			// {
			// 	// found a guy following an empty slot
			// 	// move him forward one
			// 	if(pList[i+1] != fERROR)
			// 	{
			// 		pList[i] = pList[i+1];
			// 		pList[i+1] = fERROR;
			// 	}
			// }
			
			if (pList[i] != fERROR)
			{
				CAvatar *follower;
				// get this guy
				follower = (CAvatar *)BLKPTR(pList[i]);
				// tell him to move to a new place
				follower->fFollowId.NewX = pPath[sIndex].x;
				follower->fFollowId.NewY = pPath[sIndex].y;
				follower->fFollowId.NewA = pPath[sIndex].a;
			}
		}
	}
}


