/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: aihorse.cpp  -
   Author:   Greg Hightower
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "AVATAR.HXX"
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

/* ========================================================================
   Function	- Horse
   Description - AI function to have a riderless horse run off the battle field
   Returns	 - Current state
   ======================================================================== */
void CAvatar::Horse (CAvatar *pAvatar, CAvatar::AISTATUS Status)
{
	MOVEMENT_RATE myRate;
	
	switch( Status )
	{
	case AI_INIT:
		myRate = pAvatar->mfGetMarchRate();
		pAvatar->fHorse.TargetX = (pAvatar->mfX() > 0) ? myRate : -myRate;
		
		pAvatar->Status = AI_MOVING;
		pAvatar->mfStartAnimationLoop(MARCHSEQ);
		pAvatar->hEnemy = fERROR;

		break;
			
	case AI_RELEASE:
		pAvatar->Status = AI_INIT;
		break;
	
	case AI_BEGIN_PAUSE:
	case AI_PAUSED:
	case AI_END_PAUSE:
	case AI_BEGIN_LISTEN:
	case AI_LISTEN:
	case AI_END_LISTEN:
	case AI_BEGIN_LISTEN_BOW:
	case AI_END_LISTEN_BOW:
	case AI_ROTATE_TO_CAMERA:
	case AI_RETURN_TO_POSITION:
	case AI_FALLING:
	case AI_DEAD:
	case AI_DEFEND:
	case AI_ATTACK:
	case AI_SEARCH:
			break;
	
	case AI_MOVING:		// run off the field
		//GEH check to see this horse is even remotely able to be seen
		// if not, just shoot him
		if( abs(pAvatar->mfX()) > 3000 ||
			abs(pAvatar->mfY()) > 3000 ||
			!pAvatar->mfIsVisible() )
		{
			pAvatar->DeleteAvatar(pAvatar->hThis);
			return;
		}
	
		pAvatar->mfStartAnimationOnce(MARCHSEQ);
		
		LONG newx = pAvatar->mfX() + pAvatar->fHorse.TargetX;
		LONG newy = pAvatar->mfY() - 10 + random(20);
		
		pAvatar->FaceTo(newx, newy);
		
		myRate = pAvatar->mfGetMarchRate();
		pAvatar->MoveToward(newx,newy,0,myRate);
		break;
	}
}

