#include "regen.hxx"

#include "avatar.hxx"
#include "battleui.hxx"
#include "playstat.hxx"
#include "tmsgbox.hxx"
#include "strenum.h"
#include "scnmgr.hxx"



BOOL Regenerator::fListInitted=FALSE;

static Regenerator RegenList[MAX_REGENERATORS];

void Regenerator::InitList()
{
	for (LONG i=0;i<MAX_REGENERATORS;i++)
		RegenList[i].fValid=FALSE;

	fListInitted=TRUE;
}

BOOL Regenerator::mfRegister(SHORT hAvStats,LONG time)
{
	LONG i;
	
	if (hAvStats==fERROR)
		return FALSE;
	
	if (!fListInitted)
		InitList();

	// Search for duplicates
	for (i=0;i<MAX_REGENERATORS;i++)
	{
		if (RegenList[i].fValid &&
			RegenList[i].hAvatarStats == hAvStats &&
			RegenList[i].Time == time)
		{
			// you're already in the list.
			return TRUE;
		}
	}
	
	for (i=0;i<MAX_REGENERATORS;i++)
	{
		if (!RegenList[i].fValid)
		{
			RegenList[i].hAvatarStats=hAvStats;
			RegenList[i].NextTick=0;
					// was SCENE_MGR::gTick+(((double)time)*18.2);
			RegenList[i].Time=time;
			RegenList[i].fValid=TRUE;
			return TRUE;
		}		
	}
	return FALSE;
}
	
void Regenerator::mfUnRegister(SHORT hAvStats, LONG time)
{
	for (LONG i=0;i<MAX_REGENERATORS;i++)
	{
		if (RegenList[i].fValid &&
			RegenList[i].hAvatarStats==hAvStats &&
			RegenList[i].Time == time)
		{
			RegenList[i].hAvatarStats=fERROR;
			RegenList[i].NextTick=(ULONG)-1; //max of a ulong
			RegenList[i].Time=(ULONG)-1; //max of a ulong
			RegenList[i].fValid=FALSE;
		}
	}
}

void Regenerator::Service()
{
	if (!fListInitted)
		InitList();
	
	if (fBattleCombat || fDomainTurn)	// no regen outside of adventures
		return;
		
	for (LONG i=0;i<MAX_REGENERATORS;i++)
	{
		if (RegenList[i].fValid &&
			RegenList[i].NextTick<SCENE_MGR::gTick)
		{
			RegenList[i].NextTick =
				SCENE_MGR::gTick + ((double)RegenList[i].Time)*18.2;

			if (!isGoodStats(RegenList[i].hAvatarStats))
			{
				RegenList[i].fValid=FALSE;
				continue;
			}

			DumbAutoLockPtr<PLAYER_STATS> const pPS(RegenList[i].hAvatarStats);

			if (!isGoodAvatar(pPS->hAvatar))
			{
				// Just because you didn't come on the adventure, doesn't mean
				// you have to be dropped from the list.
				// RegenList[i].fValid=FALSE;
				continue;
			}

			CAvatar * const pAvatar = (CAvatar * const) BLKPTR(pPS->hAvatar);

			if (pAvatar->attrib.KilledByLava ||
				pAvatar->attrib.KilledBySpectre)
			{
				RegenList[i].fValid=FALSE;
				continue;
			}

			if (pPS->mfGetCurHitPoints() < pPS->mfGetMaxHitPoints())
				pPS->mfHeal(1);

			// Warning this won't work for enemy AI's which might have this
			// blood ability.
			if (pAvatar->Status == CAvatar::AI_DEAD && pPS->mfGetCurHitPoints() > 0)
			{
				pAvatar->SetAIFuncIndex(CAvatar::AI_FUNC_FOLLOW_PLAYER);
				pAvatar->DoAI(CAvatar::AI_INIT);
				pAvatar->Status=CAvatar::AI_LISTEN;
				pAvatar->mfSetType( pPS->mfGetType());
			}
		}
	}
}

void Regenerator::WipeNonPlayers()
{
	for (LONG i=0;i<MAX_REGENERATORS;i++)
	{
		if (RegenList[i].fValid)
		{
			DumbHandlePtr<PLAYER_STATS> pPS(RegenList[i].hAvatarStats);

			if (isPlayer(pPS->hAvatar))
				continue;
		
			RegenList[i].fValid=FALSE;
		}
	}
}
