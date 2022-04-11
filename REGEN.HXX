#ifndef REGEN_HXX
#define REGEN_HXX 1

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_HANDLE_HXX)
#include "handle.hxx"
#endif

#if !defined(_SCNMGR_HXX)
#include "scnmgr.hxx"
#endif

#if !defined(_SCENE_HXX)
#include "scene.hxx"
#endif

#if !defined(_ADVENTUR_HXX)
#include "adventur.hxx"
#endif


#define MAX_REGENERATORS 20


class Regenerator
{
	private:
		static BOOL fListInitted;
		SHORT hAvatarStats;
		ULONG  NextTick;
		ULONG  Time;
		BOOL  fValid;
	public:
		static void InitList();
		static void WipeNonPlayers();
		static void Service();
		static BOOL mfRegister(SHORT /* hAvatarStats */,LONG /* time */);
		static void mfUnRegister(SHORT /* hAvatarStats */, LONG /* time */);
};

inline BOOL isPlayer(SHORT hAvatar);
inline BOOL isGoodAvatar(SHORT hAvatar);



inline BOOL isPlayer(SHORT hAvatar)
{
	if (hAvatar==fERROR)
		return FALSE;


	ADVENTURER_TEAM_ITOR AdvItor;
	for (AdvItor = ADVENTURER::begin(); AdvItor != ADVENTURER::end();
		AdvItor++)
	{
		if (hAvatar==*AdvItor)
			return TRUE;
	}
	return FALSE;
}


inline BOOL isGoodAvatar(SHORT hAvatar)
{
		if (hAvatar==fERROR)
			return FALSE;

		DumbHandlePtr<SCENE> pScene(SCENE_MGR::hCurrentScene);

		for (LONG i=0;i<MAX_AVATARS;i++)
		{
			if (pScene->Avatars[i]==hAvatar)
				return TRUE;
		}
		return FALSE;
}	
#endif