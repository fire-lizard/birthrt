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
#include "avatar.hxx"
#include "sound.hxx"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define MAX_TRAVEL_DISTANCE 2000
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
/* ========================================================================
   Function	- Stunned
   Description - AI function to randomly move an avatar.
   Returns	 - void 
   ======================================================================== */
void CAvatar::Stunned (CAvatar *pAvatar, CAvatar::AISTATUS Status)
{
	LONG FloorHeight;
	LONG CeilingHeight;
	LONG Special;
	
	if( pAvatar->Status != AI_DEAD &&
		player.ThingIndex != fERROR &&
		!reject( pAvatar->mfGetSector(), mythings[player.ThingIndex].sect))
	{
		return;
	}

	if(pAvatar->Status != AI_DEAD)
	{
		pAvatar->mfCheckCrush();
	}
	
	if(pAvatar->Status == AI_DEAD)
		Status = AI_DEAD;
	
	switch( Status )
	{
		case AI_INIT:
		{
			LONG deltaZ;
		
			pAvatar->Status = AI_MOVING;
			pAvatar->attrib.IssuedChallange = FALSE;
			pAvatar->fDamageFlag = 1;
			
			pAvatar->fStunned.Movement.dx = 0;
			pAvatar->fStunned.Movement.dy = pAvatar->mfGetMarchRate();
			pAvatar->fStunned.Movement.dz = 0;
		
			pAvatar->fStunned.Home.x = pAvatar->mfX();
			pAvatar->fStunned.Home.y = pAvatar->mfY();
			pAvatar->fStunned.Home.z = pAvatar->mfZ();
		
			//3 randoms so there is a chance to go negative.
			pAvatar->fStunned.Target.x = random(MAX_TRAVEL_DISTANCE);//-random(MAX_TRAVEL_DISTANCE)-random(MAX_TRAVEL_DISTANCE);
			pAvatar->fStunned.Target.y = random(MAX_TRAVEL_DISTANCE);//-random(MAX_TRAVEL_DISTANCE)-random(MAX_TRAVEL_DISTANCE); 
			pAvatar->fStunned.Target.z = 0;
		
			Rotate((POINT *)(&(pAvatar->fStunned.Target)), pAvatar->mfAngle());
			
			pAvatar->fStunned.Target.x += pAvatar->fStunned.Home.x;
			pAvatar->fStunned.Target.y += pAvatar->fStunned.Home.y;
			pAvatar->fStunned.Target.z += pAvatar->fStunned.Home.z;
				
			pAvatar->hEnemy=fERROR;
			
			pAvatar->Status = CAvatar::AI_MOVING;
			pAvatar->mfStartAnimationLoop(WALKSEQ);
			
			

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
			// Dead is dead. Don't move.
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

			if(pAvatar->mfTestSequenceDone() &&
			     pAvatar->CurSequence != DEADSEQ)
			{
				AddSndObj(SND_BODY_FALLS_DEAD1, SND_BODY_FALLS_DEAD_TOTAL, -1 /* pAvatar->ThingIndex */);
				pAvatar->mfPlayAnimationOnce(DEADSEQ);
				
				pAvatar->attrib.DiedLastFrame=TRUE;
				pAvatar->mfTurnHighlightOff();

				if(pAvatar->attrib.KilledBySpectre == TRUE)
				{
					pAvatar->mfTurnToSpectre();
					pAvatar->attrib.KilledBySpectre = FALSE;
				}

			}
			if (pAvatar->mfTestSequenceDone() &&
				pAvatar->mfGetArtSequence() != DEADSEQ)
				pAvatar->mfPlayAnimationOnce(DEADSEQ);
			break;
		case AI_MOVING:
		{
			//approximately 1 in 5 calls (frames?) change the avatar's target

			if (random(5)==3) //1 in 5 chance
			{
				pAvatar->fStunned.Target.x = random(MAX_TRAVEL_DISTANCE);//-random(MAX_TRAVEL_DISTANCE)-random(MAX_TRAVEL_DISTANCE);
				pAvatar->fStunned.Target.y = random(MAX_TRAVEL_DISTANCE);//-random(MAX_TRAVEL_DISTANCE)-random(MAX_TRAVEL_DISTANCE); 
				pAvatar->fStunned.Target.z = 0;
			
				Rotate((POINT *)(&(pAvatar->fStunned.Target)), pAvatar->mfAngle());
				
				pAvatar->fStunned.Target.x += pAvatar->fStunned.Home.x;
				pAvatar->fStunned.Target.y += pAvatar->fStunned.Home.y;
				pAvatar->fStunned.Target.z += pAvatar->fStunned.Home.z;
			}
		
		
			LONG distance = aprox_dist(0,0,pAvatar->fStunned.Target.x,pAvatar->fStunned.Target.y);
		
			LONG FloorHeight;
			LONG CeilingHeight;
			LONG Special;
			pAvatar->mfGetSectorInfo(&FloorHeight, &CeilingHeight, &Special);
			
			LONG Angle=0;
			PLAYER TempPlayer;
			FIXED_VECTOR_3D NewVector, OldVector;
			const LONG Rate=pAvatar->mfGetMarchRate();
			WadThingType Result = iNOTHING;
			LONG BumpDistance;
			LONG dz,dx,dy ;
			LONG BumpIndex;


		
			WadThingType bump;
		
			bump=pAvatar->mfMoveTowardWithBump(distance,
							pAvatar->fStunned.Target.x,
							pAvatar->fStunned.Target.y,
							20,
							Rate,
							(SHORT)((TRUE == pAvatar->mfStayInMonsterBox()) 
										? CHECKLINE_MONSTER : 0),
							&Angle,
							&BumpIndex);
		}	
	}
}

/* ========================================================================
   Function	- mfSwitchToStunned
   Description - Stun the guy
   Returns	 - void
   ======================================================================== */
   
void STUNNED_DATA::mfSwitchToStunned(LONG hAvatar)
{
	CAvatar * const pAvatar = (CAvatar * const) BLKPTR((SHORT) hAvatar);
	pAvatar->fPrev.Status = CAvatar::AI_INIT;
	pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_STUNNED);
	pAvatar->Status = CAvatar::AI_INIT;
	
	if (pAvatar->hEnemy != fERROR)
	{
		CAvatar * const pEnemy = (CAvatar * const) BLKPTR(pAvatar->hEnemy);
		pEnemy->mfDisEngage();
	}
}
