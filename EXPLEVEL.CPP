/* =======================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   =======================================================================
   Filename: EXPLEVEL.CPP  - extra effects of experience and levels
   Author:   Donald Tsang

   =======================================================================

   Contains the following general functions:

   ExpCheck() - Handles level gaining
   GainExp()  - Give experience pts to a character.
   SaveVs() -  Rolls saving throws
   ======================================================================= */

/* -----------------------------------------------------------------------
   Includes, Defines, Macros, and other Pre-Processor stuff
   ----------------------------------------------------------------------- */
#include <stdlib.h>

#ifdef _WINDOWS
#include <windows.h>
#endif
#include "SYSTEM.H"
#include "HANDLE.HXX"
#include "PLAYCLAS.HXX"
#include "PLAYSTAT.HXX"
#include "EXPLEVEL.HXX"
#include "STATSMOD.HXX"
#include "CONSTITN.HXX"
#include "INVNTORY.HXX"
#include "REGENTS.HXX"
#include "REPORT.HXX"
#ifdef _WINDOWS
#include "WINSYS\MULPLAY.HXX"
#endif

// Its in stdlib.h
#ifndef _WINDOWS
#define min(a,b)  ((a>b)?(b):(a))
#endif

/* -----------------------------------------------------------------------
   Prototypes
   ----------------------------------------------------------------------- */

BOOL	ExpCheck(PLAYER_STATS *pPS);
void	GainExp(PLAYER_STATS *pPS, const LONG ExpAmount,
				const BOOL CanGainLevelNow);
void GainRealmExp(LONG RegentIndex, const LONG ExpAmount,
				const BOOL CanGainLevelNow, const BOOL fSend);
BOOL	SaveVs (const SHORT hPlayerStats, SAVETYPE savetype,
				SBYTE modifier);
UBYTE	lookup_save(SCLASSTYPE superclass, SAVETYPE savetype);

/* -----------------------------------------------------------------------
   Global Variables
   ----------------------------------------------------------------------- */

static const SAVETABLE saving_throw[] = {
    // Warriors (Fighters, Paladins, Rangers)
    {2, 17, {{14, 13, 11, 10,  8,  7,  5,  4,  3},     // Para/Poison/Death
             {16, 15, 13, 12, 10,  9,  7,  6,  5},     // Rod/Staff/Wand
             {15, 14, 12, 11,  9,  8,  6,  5,  4},     // Petrify/Polymorph
             {17, 16, 13, 12,  9,  8,  5,  4,  4},     // Breath Weapon
             {17, 16, 14, 13, 11, 10,  8,  7,  6}}  }, // Spells

    // Priests (Clerics, Druids)
    {3, 19, {{10,  9,  7,  6,  5,  4,  2},
             {14, 13, 11, 10,  9,  8,  6},
             {13, 12, 10,  9,  8,  7,  5},
             {16, 15, 13, 12, 11, 10,  8},
             {15, 14, 12, 11, 10,  9,  7}}  },

    // Rogues (Thieves, Bards)
    {4, 21, {{13, 12, 11, 10,  9,  8},
             {14, 12, 10,  8,  6,  4},
             {12, 11, 10,  9,  8,  7},
             {16, 15, 14, 13, 12, 11},
             {15, 13, 11,  9,  7,  5}}  },

    // Wizards (Mages, Magicians, Illusionists)
    {5, 21, {{14, 13, 11, 10,  8},
             {11,  9,  7,  5,  3},
             {13, 11,  9,  7,  5},
             {15, 13, 11,  9,  7},
             {12, 10,  8,  6,  4}}  }
};

/* -----------------------------------------------------------------------
   Code
   ----------------------------------------------------------------------- */
UBYTE lookup_save(SCLASSTYPE classtype, SHORT level, SAVETYPE savetype)
{
    UBYTE save;
    SHORT best_save_level, i, index, increment;

    best_save_level = saving_throw[classtype].best_save_level;
    increment = saving_throw[classtype].levels_per_advance;

    if (level > best_save_level)  // maximum advancement
        level = best_save_level;

    index = 0;
    for (i=1; i+increment < level; i+=increment)
        index++;

    return saving_throw[classtype].type[savetype].save[index];
}

SCLASSTYPE sclass_by_class(PLAYER_CLASS_INFO::TYPE pclass)
{
    SCLASSTYPE sclass;

    switch(pclass)
    {
    case PLAYER_CLASS_INFO::FIGHTER :
    case PLAYER_CLASS_INFO::PALADIN :
    case PLAYER_CLASS_INFO::RANGER :
        sclass = SC_WARRIOR;
        break;

    case PLAYER_CLASS_INFO::PRIEST :
        sclass = SC_PRIEST;
        break;

    case PLAYER_CLASS_INFO::THIEF :
    case PLAYER_CLASS_INFO::BARD :
        sclass = SC_ROGUE;

    case PLAYER_CLASS_INFO::WIZARD :
    case PLAYER_CLASS_INFO::MAGICIAN :
        sclass = SC_WIZARD;
        break;

    default:
        sclass = SC_WARRIOR;
    }

    return sclass;
}


BOOL SaveVs(const SHORT hPlayerStats, SAVETYPE savetype, SBYTE modifier)
{
    SHORT i, level, best_save = 25, save;
    DICE SaveDice;
    BOOL result;

    if (hPlayerStats == fERROR)		// you have no stats; you lose.
    	return FALSE;
    	
    PTR_PLAYER_STATS pPS = (PTR_PLAYER_STATS) BLKPTR(hPlayerStats);
    SBYTE global_modifier = pPS->StatsMod.mfGetMod(STATSMOD::SAVES_MOD);

    DICE const * const TheHitDice = pPS->mfGetHitDice();

    if (TheHitDice->sbNumberOfDice > 0)         /* is Monster */
    {
        level = TheHitDice->sbNumberOfDice;
        if (TheHitDice->sbModifier > 0)
            level += (TheHitDice->sbModifier + 3) / 4;
        save = lookup_save(SC_WARRIOR, level, savetype);
        if (best_save > save)
            best_save = save;
    }

    for (i=0; i < MAX_PLAYER_CLASSTYPES; i++)
    {
        if (pPS->mfGetLevel(i) > 0)  // guy is of this class
        {
            save = lookup_save(sclass_by_class(pPS->mfGetClassType(i)),
                               pPS->mfGetLevel(i), savetype);
            if (best_save > save)
                best_save = save;
        }
    }

    SaveDice.sbNumberOfDice = 1;
    SaveDice.sbNumberOfSides = 20;
    SaveDice.sbModifier = 0;
    save = RollDice(&SaveDice);

    if (save == 1)        // natural 20 always fails
    {
        printf("Rolled a natural 1.  Good thing there's no fumbles.\n");
        result = FALSE;
    }
    else if (save == 20)  // natural 20 always succeeds
    {
        printf("Rolled a natural 20.  Automatic Success\n");
        result = TRUE;
    }
    else
    {
        result = (save + modifier + global_modifier >= best_save);

        printf("Rolled a %d, modified to %d\nThis was ",
            save, save+modifier+global_modifier);
        if (!result) printf("not ");
        printf("a successful save (needed %d)\n", best_save);
    }

    return result;
}


BOOL ExpCheck(PLAYER_STATS * pPS)
{
// [d5-28-97 JPC] Added checks for player class as well as level.
    BOOL	has_gained_level = FALSE;
    BOOL	isWarrior;
    SHORT	index, num_classes;

    PTR_PLAYER_CLASS_LIST		pCL;
	PLAYER_CLASS_INFO::TYPE		charclass;
	SBYTE						charlevel;
	LONG						xp;
    LONG						xp_next;

    if (pPS == NULL)
    {
#if defined(_DEBUG)
        fatal_error("pPS is NULL in ExpCheck!");
#endif
        return FALSE;
    }

    num_classes = 0;
    isWarrior = FALSE;
    for (index = 0; index < MAX_PLAYER_CLASSTYPES; index++)
    {
        if (pPS->mfGetClassType(index) != PLAYER_CLASS_INFO::NOT_SET &&
           pPS->mfGetLevel(index) > 0)
        {
            num_classes++;
            if (sclass_by_class(pPS->mfGetClassType(index)) == SC_WARRIOR)
                isWarrior = TRUE;
        }
    }

    pCL = &pPS->PlayerClassList;
    for (index = 0; index < MAX_PLAYER_CLASSTYPES; index++)
    {
        if (pPS->mfGetClassType(index) != PLAYER_CLASS_INFO::NOT_SET &&
			   pPS->mfGetLevel(index) > 0)
        {
			  charclass = pCL->mfGetType(index);
			  charlevel = pCL->Class[index].sbLevel;
			  xp    = pCL->mfGetExpPts(index);
	
			  if (charlevel < 1 || charlevel > 19)  // can't gain levels here
					continue;
	
			  xp_next = ALL_CLASS_EXPERIENCE_TABLE::
					mfGetMinimumExperience(charclass, charlevel+1);
	
			  if (xp > xp_next)
			  {
					DICE oldHD, newHD, hdDif;
					DICE const *d;
					SBYTE result;
	
	/* gain some hitpoints */
					d = PLAYER_CLASS_INFO::mfGetDice(charclass, charlevel);
					oldHD.sbNumberOfDice = d->sbNumberOfDice;
					oldHD.sbNumberOfSides = d->sbNumberOfSides;
					oldHD.sbModifier = d->sbModifier;
	
					d = PLAYER_CLASS_INFO::mfGetDice(charclass, charlevel+1);
					newHD.sbNumberOfDice = d->sbNumberOfDice;
					newHD.sbNumberOfSides = d->sbNumberOfSides;
					newHD.sbModifier = d->sbModifier;
	
					hdDif.sbNumberOfDice  = newHD.sbNumberOfDice - oldHD.sbNumberOfDice;
					hdDif.sbNumberOfSides = newHD.sbNumberOfSides;
					hdDif.sbModifier      = newHD.sbModifier - oldHD.sbModifier;
				
					result = RollDice(&hdDif);
					if (hdDif.sbNumberOfDice > 0) /* Con bonus only for hit _dice_ */
					{
						if (isWarrior)
							result += CONSTITUTION::mfGetWarriorHitPtBonus(pPS->mfGetCon()) *
										hdDif.sbNumberOfDice;
						else
							result += CONSTITUTION::mfGetHitPtAdjustment(pPS->mfGetCon()) *
										hdDif.sbNumberOfDice;
					}
				
					result /= num_classes;  /* multiclass: divide, rounding down  */
					if (result < 1)         /* ... but never less than 1 hp/level */
						result = 1;
				
					pPS->FullHitPts += result;
					pPS->HitPts += result;
	
	/* go up a level */
					pCL->Class[index].sbLevel++;
	
					if (pPS->Thac0 > pCL->mfGetBestThac0())  // THAC0 improved
						pPS->Thac0 = pCL->mfGetBestThac0();
						
					// fix the regents array entry too, sigh...
					{
						SHORT regentnum;
						for (regentnum = 1; regentnum < CHARACTER_COUNT; regentnum++)
						{
							if (playerstats[regentnum] == pPS->hThis)
								break;
						}
						if (regentnum < CHARACTER_COUNT)	// found it
						{
							AddReport(IMMEDIATE_LEVELUP, regentnum, regents[regentnum].mfGetRealm(), 0);
							if (index == 0)
								regents[regentnum].mfIncLevel1(1);
							else
								regents[regentnum].mfIncLevel2(1);
						}
					}
	
					has_gained_level = TRUE;
				pPS->Inventory.AddPriestSpells(pPS->hThis);
			}
		 }
    }

    return has_gained_level;
}


void GainRealmExp(LONG RegentIndex, const LONG ExpAmount,
				const BOOL CanGainLevelNow, const BOOL fSend)
{
#ifdef _WINDOWS
	if ( IsMultiPlayer() && fSend )
	{
		AMultiPlayer.GainExpMessage(RegentIndex, ExpAmount, CanGainLevelNow );
	}
#endif	

	DumbAutoLockPtr< PLAYER_STATS > const pPS(playerstats[RegentIndex]);
		
	GainExp(pPS, ExpAmount, CanGainLevelNow);
}

void GainExp(PLAYER_STATS * pPS, const LONG ExpAmount,
             const BOOL CanGainLevelNow)
{
// [d5-28-97 JPC] Added checks for player class as well as level.
    PLAYER_CLASS_LIST::PLAYER_CLASS_DATA *pCD;

    SHORT index, num_classes;
    SHORT current_level;
    LONG  current_xp;
    LONG  xp_for_next_level;
    LONG  xp_for_yet_another_level;

    num_classes = 0;
    for (index = 0; index < MAX_PLAYER_CLASSTYPES; index++)
        if (pPS->mfGetClassType(index) != PLAYER_CLASS_INFO::NOT_SET &&
			   pPS->mfGetLevel(index) > 0)
            num_classes++;

    if (!num_classes)  /* monsters don't gain xp */
        return;

    for (index = 0; index < MAX_PLAYER_CLASSTYPES; index++)
    {
        if (pPS->mfGetClassType(index) != PLAYER_CLASS_INFO::NOT_SET &&
			   pPS->mfGetLevel(index) > 0)
        {
            pCD = &(pPS->PlayerClassList.Class[index]);

            xp_for_next_level =
               ALL_CLASS_EXPERIENCE_TABLE::mfGetMinimumExperience(pCD->Type,
                   min(pCD->sbLevel+1, MAX_EXPERIENCE_LEVELS));

            xp_for_yet_another_level =
               ALL_CLASS_EXPERIENCE_TABLE::mfGetMinimumExperience(pCD->Type,
                   min(pCD->sbLevel+2, MAX_EXPERIENCE_LEVELS));

            /*-------------------------------------------------*/
            /* split experience amongst the individual classes */
            pCD->ExperiencePts += ExpAmount / num_classes;
            /*-------------------------------------------------*/
            printf("Class %d: +%d experience, now %d\n", index,
                ExpAmount / num_classes, pCD->ExperiencePts);

            // Can't gain more than one level
            if ((pCD->ExperiencePts > xp_for_yet_another_level) &&
                (xp_for_yet_another_level > xp_for_next_level))
            {
                pCD->ExperiencePts = xp_for_yet_another_level - 1;
                printf("With this much experience, could gain more "
                       "than one level!\n");
                printf("Chopping XP to %d\n", pCD->ExperiencePts);
            }

            if (CanGainLevelNow && pCD->ExperiencePts > xp_for_next_level)
                ExpCheck(pPS);
        }
    }
}

