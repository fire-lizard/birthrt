/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: playstat.cpp  -
   Author:   Gary Powell

   ========================================================================

   Contains the following general functions:


   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <string.h>
#include <ctype.h>

#include "AVATAR.HXX"
#include "GAMEMAP.HXX"
#include "INVNTORY.HXX"
#include "PLAYSTAT.HXX"
#include "SPELLPRO.HXX"
#include "WISDOM.HXX"
#include "strenum.h"
#include "SCNMGR.HXX"
#include "EXPLEVEL.HXX"
#include "REGEN.HXX"
#include "REGENTS.HXX"


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
extern BOOL fStartMultiGame;

DECL_VECTOR_DATA(HDL_PLAYER_STATS,playerstats,CHARACTER_COUNT);
BOOL playerStatsInitted = FALSE;

// [d5-05-97 JPC] KLUDGE structure to allow users to change
// bloodline data with the character editor program.
// Information is posted to "regents" array (class type REGENT).
struct tagBloodline {
	LONG	BL_grade;
	LONG  BL_deriv;
	LONG  BL_strength;
} gBloodInfo;

// [d5-14-97 JPC] Table needed to translate from PLAYSTAT class enums
// to GAMEMAP class enums.
static CLASS_ENUM gaClassTranslate[] = {FIGHTER, PALADIN, RANGER, PRIEST, WIZARD, WIZARD, THIEF, BARD};

// [d5-30-97 JPC] A cross-reference table to modify regent's icon based
// on the THINGTYPE character art, which can change if the user edits the
// character.
struct tagIconCrossRef {
	THINGTYPE	ttype;
	MAP_ICON		icon;
} gIconCrossRef[] = {
	DWARF_LORD_1,    REG_DLORD1_UNIT_ICON,
	DWARF_LORD_2,    REG_DLORD2_UNIT_ICON,
	DWARF_LORD_3,    REG_DLORD3_UNIT_ICON,
	GUILDER_FEMALE,  REG_GILDF1_UNIT_ICON,
	GUILDER_MALE_1,  REG_GILDM1_UNIT_ICON,
	GUILDER_MALE_2,  REG_GILDM2_UNIT_ICON,
	LORD_FEMALE_1,   REG_LORDF1_UNIT_ICON,
	LORD_FEMALE_2,   REG_LORDF2_UNIT_ICON,
	LORD_FEMALE_3,   REG_LORDF3_UNIT_ICON,
	LORD_MALE_1,     REG_LORDM1_UNIT_ICON,
	LORD_MALE_2,     REG_LORDM2_UNIT_ICON,
	LORD_MALE_3,     REG_LORDM3_UNIT_ICON,
	LORD_MALE_4,     REG_LORDM4_UNIT_ICON,
	LORD_MALE_5,     REG_LORDM5_UNIT_ICON,
	LORD_MALE_6,     REG_LORDM6_UNIT_ICON,
	LORD_MALE_7,     REG_LORDM7_UNIT_ICON,
	PRIEST_FEMALE_1, REG_PRSTF1_UNIT_ICON,
	PRIEST_MALE_1,   REG_PRSTM1_UNIT_ICON,
	PRIEST_MALE_2,   REG_PRSTM2_UNIT_ICON,
	WIZARD_FEMALE_1, REG_WIZDF1_UNIT_ICON,
	WIZARD_FEMALE_2, REG_WIZDF2_UNIT_ICON,
	WIZARD_MALE_1,   REG_WIZDM1_UNIT_ICON,
	WIZARD_MALE_2,   REG_WIZDM2_UNIT_ICON,
	WIZARD_MALE_3,   REG_WIZDM3_UNIT_ICON,
	WIZARD_MALE_4,   REG_WIZDM4_UNIT_ICON,
	ELF_LORD_1,      REG_ELORD1_UNIT_ICON,
	ELF_LORD_2,      REG_ELORD2_UNIT_ICON,
	ELF_LADY_LORD,   REG_ELADY1_UNIT_ICON
};

int gcCrossRefs = sizeof (gIconCrossRef) / sizeof (gIconCrossRef[0]);

/* ------------------------------------------------------------------------
   Local Variables
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Code
   ------------------------------------------------------------------------ */

/* ========================================================================
   Function    - mfInitValues
   Description - Set all the values to a default value.
   Returns     -
   ======================================================================== */

void PLAYER_STATS::mfInitValues()
{
	StructVersion = CURRENT;
	ThingType = NO_THING;
	Ability.mfInitData();
	Race = RACE_INFO::NOT_SET;
	// [d4-09-97, 4-23-97 JPC] NameClass has been replaced by szNameStat,
	// which holds the actual name.  (This is done to allow users to change
	// the names with a character editor.)
	//	Name functions are:
	//		mfGetName
	//		mfSetName
	// NameClass.fName = PLAYER_NAME::NOT_SET;
	mfSetName ("");
	TitleClass.fTitle = PLAYER_TITLE::NOT_SET;
	DesClass.fDescription = PLAYER_DESCRIPTION::NOT_SET;
	Alignment = ALIGNMENT_INFO::NON_ALIGNED;
	memset(&StatsMod, 0, sizeof(StatsMod));
	HitPts = 0;
	sbArmorClass = 10;
	DefaultArmor = ARMOR_INFO::NO_ARMOR;
	DefaultWeapon = WEAPON_INFO::NO_WEAPON;
	sMorale = 0;
	FullHitPts = 0;
	Thac0 = 0;
	memset(DamageDice, 0, sizeof(DamageDice));
	memset(&HitDice, 0, sizeof(HitDice));
	XP = 0;
	PlayerClassList.mfInitData();
	hAvatar=fERROR;
	NumberOfAttacks = 1;
	Gold = 0;
	Blindness = FALSE;
	Paralyized = FALSE;
	
	//ShieldType = SHIELD_INFO::NO_SHIELD;
	//Task[0] = 0;
	//IdHeir = 0;
	//Age = 0;
	//Height = 0;
	//Weight = 0;
	
	//this should definitly be done outside of this function.
	//printf("initting invn with stats handle %li and ID %li\n",hResult,ID);
	//Inventory.mfInit(hResult,ID);

}

/* ========================================================================
   Function    - mfAllocateSpace
   Description - Make space using the memory manager
   Returns     - Handle to the PLAYER_STATS
   ======================================================================== */

HDL_PLAYER_STATS PLAYER_STATS::mfAllocateSpace(SHORT hAvatar,LONG ID)
{
	HDL_PLAYER_STATS hResult = NewBlock(sizeof(PLAYER_STATS));
	
	if (hResult != fERROR)
	{
		DumbAutoLockPtr <PLAYER_STATS > const pPlayerStats(hResult);
		pPlayerStats->mfInitValues();
		pPlayerStats->hAvatar = hAvatar;
		pPlayerStats->hThis = hResult;
		
#if defined (MEMORY_CHK)
		if (ID != fERROR)
		{
			char cpBuffer[20];
			
			sprintf(cpBuffer, "%ld", ID);
			SetBlockName(cpBuffer,hResult);
		}
#endif
	}
	
	return hResult;
}

/* ========================================================================
   Function    - mfReadTextData
   Description - Read a .ava file and fill in the values
   			 	 compute any values necessary from the .ava data.
   Returns     - Status of last file read().
   ======================================================================== */

LONG PLAYER_STATS::mfReadTextData(FILE *fp)
{
	LONG Result;
	LONG lTemp;
	LONG lTemp2;
	LONG lTemp3;
	LONG lTemp4;
	LONG lTemp5;
	LONG lTemp6;
	char	cpBuffer[126];
	LONG FileVersion;
	
	
	if (EOF != GetNextLine(fp, cpBuffer, sizeof(cpBuffer)))
	{
		sscanf(cpBuffer, "%ld", &FileVersion);
	}
	else
	{
		printf("WARNING: No File version in AVA file.\n");
		goto Leave_fn;
	}
	
	if (EOF != GetNextLine(fp, cpBuffer, sizeof(cpBuffer)))
	{
		sscanf(cpBuffer, "%ld", &lTemp);
		ThingType = (THINGTYPE)lTemp;
	}
	else
	{
		printf("WARNING: No ThingType Id in AVA file.\n");
		ThingType = NO_THING;
		goto Leave_fn;
	}
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		printf("WARNING: Missing  AVA Version number.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &lTemp);		/* PLAYER_STATS::VERSION  */
	StructVersion = (PLAYER_STATS::VERSION) lTemp;
	
	Result = Ability.mfReadData(fp);							// ABILITY
	if (EOF == Result)
	{
		printf("WARNING: Missing AVA Ability info.\n");
		goto Leave_fn;
	}
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		printf("WARNING: Missing AVA player race info.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &Race);				/* PLAYER_RACE_TYPE */	
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		printf("WARNING: Missing player name.\n");
		goto Leave_fn;
	}
	// [d4-08-97, 4-23-97 JPC] AVA files now contain the name.
	// sscanf(cpBuffer, "%ld", &lTemp);
	// PLAYER_STATS::NameClass.fName =(PLAYER_NAME::NAME) lTemp;
	// If you don't want a name, store it as '' or "" in the AVA file.
	if (strcmp (cpBuffer, "''") != 0 && strcmp (cpBuffer, "\"\"") != 0)
		mfSetName (cpBuffer);
	else
		mfSetName ("");

	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		printf("WARNING: Missing player title.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &lTemp);
	PLAYER_STATS::TitleClass.fTitle =(PLAYER_TITLE::TITLE) lTemp;

	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		printf("WARNING: Missing player Name.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &lTemp);
	PLAYER_STATS::DesClass.fDescription =(PLAYER_DESCRIPTION::DES) lTemp;

	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		printf("WARNING: Missing player alignment data.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &lTemp);			/* ALIGNMENT_TYPE */	
	Alignment = (ALIGNMENT_INFO::TYPE) lTemp;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		printf("WARNING: Missing player hit point data in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%hd", &HitPts);				/* LONG */	
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		printf("WARNING: Missing Armor Class information in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &lTemp);			/* LONG */	
	sbArmorClass = (SBYTE) lTemp;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		printf("WARNING: Missing Armor type information in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &lTemp);			/* ARMOR_TYPE */	
	DefaultArmor = (ARMOR_INFO::TYPE) lTemp;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		printf("WARNING: Missing Weapon type information in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &lTemp);			/* WEAPON_TYPE */	
	DefaultWeapon = (WEAPON_INFO::TYPE) lTemp;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		printf("WARNING: Missing Morale data in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%d",  &sMorale);
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		printf("WARNING: Missing Full HitPt data in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%hd", &FullHitPts);
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		printf("WARNING: Missing Thac0 data in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &Thac0);
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		printf("WARNING: Missing Damage Dice data in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "(%ld %ld %ld) (%ld %ld %ld)",
	                   &lTemp, &lTemp2, &lTemp3, &lTemp4, &lTemp5, &lTemp6);
	DamageDice[0].sbNumberOfDice = (SBYTE) lTemp;
	DamageDice[0].sbNumberOfSides = (SBYTE) lTemp2;
	DamageDice[0].sbModifier = (SBYTE) lTemp3;
	DamageDice[1].sbNumberOfDice = (SBYTE) lTemp4;
	DamageDice[1].sbNumberOfSides = (SBYTE) lTemp5;
	DamageDice[1].sbModifier = (SBYTE) lTemp6;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		printf("WARNING: Missing Hit Dice data in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "(%ld %ld %ld)", &lTemp, &lTemp2, &lTemp3);
	HitDice.sbNumberOfDice = (SBYTE) lTemp;
	HitDice.sbNumberOfSides = (SBYTE) lTemp2;
	HitDice.sbModifier = (SBYTE) lTemp3;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		printf("WARNING: Missing Experience Pts data in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &XP);
	
	Result = PlayerClassList.mfReadTextData(fp);
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		printf("Warning: Missing number of Attacks in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer,"%ld", &lTemp);
	NumberOfAttacks = (SBYTE) lTemp;

	// [d5-05-97 JPC] KLUDGE: read in bloodline info if present.
	// It is NOT an error if the info is not present.  The only
	// way for it to be in the AVA file is if someone used the
	// character editor program to modify the AVA file.
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF != Result)
	{
		sscanf(cpBuffer,"%ld", &gBloodInfo.BL_grade);
		Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
		if (EOF != Result)
		{
			sscanf(cpBuffer,"%ld", &gBloodInfo.BL_deriv);
			Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
			if (EOF != Result)
			{
				sscanf(cpBuffer,"%ld", &gBloodInfo.BL_strength);
			}
		}
	}
	
	// Now Calculate the other fields as necessary.
	if (HitPts == 0)
	{
		if (HitDice.sbNumberOfDice != 0)
		{
			HitPts = RollDice(&HitDice);
		}
		else
		{
			HitPts = PlayerClassList.mfRollHitPts();
		}
		
	}
	
	if (FullHitPts == 0)
	{
		FullHitPts = HitPts;
	}
	
	if (DefaultArmor != ARMOR_INFO::NO_ARMOR)
	{
		sbArmorClass = ARMOR_INFO::mfGetArmorClass(DefaultArmor);
	}
	
	//GEH PATCH1
	//GEH fix for bad data send in production AVA files
	if ( Thac0 == 16 )
		Thac0 = 0;
	
	if (Thac0 == 0)
	{
		Thac0 = PlayerClassList.mfGetBestThac0();
	}
	
Leave_fn:
	return Result;
}

/* ========================================================================
   Function    - PLAYER_STATS::mfGetSpellCastClass
   Description - determines whether either of the character's classes can
                 cast spells
   Returns     - 1 for class 1, 0 for class 0, CANT_CAST_SPELLS otherwise
   ======================================================================== */
SHORT const PLAYER_STATS::mfGetSpellCastClass(
						SPELL_INFO::CASTERTYPE const SpellCaster) const
{
	LONG i=0;
	
	for (i=0;i<MAX_PLAYER_CLASSTYPES;i++)
		if (num_spells_of(SpellCaster,SPELL_INFO::LEVEL_1,mfGetClassType(i),mfGetLevel(i)))
			return i;

	return CANT_CAST_SPELLS;
}

/* ========================================================================
   Function    - PurgePS
   Description - Purge the PLAYER_STATS array.
   Returns     -
   ======================================================================== */

void PurgePS(LONG count)
{
	PTR_PLAYER_STATS pPS;
	LONG i;
	
	Regenerator::InitList();
	ItemEffects::WipeActiveItems();
	DisposeResExtension ( "AVD" );	// GEH 9/97 remove any trace from resource manager
	for (i=1; i <count; i++)
	{
		if (playerstats[i] == fERROR)
			continue;
		pPS = (PTR_PLAYER_STATS) BLKPTR(playerstats[i]);
		pPS->Inventory.mfDeinit();
		DisposBlock(playerstats[i]);
		playerstats[i] = fERROR;
	}
	playerStatsInitted = FALSE;  /* else double AVA load will fail */
	return;
}

/* ========================================================================
   Function    - StatReadFromAVA
   Description - Build the PLAYER_STATS array
   Returns     -
   ======================================================================== */

void StatReadFromAVA(BOOL SaveAfter)
{
    LONG 		i;
	 MAP_ICON 	oldIcon, newIcon;		// [d5-30-97 JPC]
    int        icref;               // [d5-30-97 JPC]

    printf("loading stats from AVA files.\n");


    for (i=1; i < CHARACTER_COUNT; i++)
    {
		  // [d5-05-97 JPC] KLUDGE ALERT!
		  // The character editor lets you change the bloodline info;
		  // the standard AVA file does NOT store this info.  It is
		  // only stored in the "regents" array.
		  // Solution: tack the bloodline information onto the end of the
		  // AVA file (if it is saved from the character editor program).
		  // If the information is present, use it to update a global
		  // structure, gBloodInfo, and post the information here.
		  // (Note that the character editor only lets you edit existing
		  // regents.  You can't add new ones.  Therefore, the realms and
		  // the IDs of the regents cannot and do not change.)
        gBloodInfo.BL_strength = -1;   // -1 means: not in AVA file


		  playerstats[i] = LoadStats(regents[i].mfGetid(), fERROR);
        if (playerstats[i] == fERROR)
        {
//#if defined(_DEBUG)
            fatal_error("Can't load stats from AVA files!");
//#endif
            PurgePS(i);
            return;
        }
        DumbAutoLockPtr < PLAYER_STATS > const pPS (playerstats[i]);

		// [d5-14-97 JPC] Update regents array with race and class
		// data from AVA file (which is now in the player stats).
		// NOTE: Do not change regents data if the AVA data is NOT_SET (-1).
		// NOTE: Do not change regents data if the ID is:
		//	1049	Gorgon
		//	1054	Rhuobhe
		//	1055	Nhoun
		//	1056	Spider
		//	1999	Generic

		if (regents[i].mfGetid() != 1049 &&
		    regents[i].mfGetid() != 1054 &&
		    regents[i].mfGetid() != 1055 &&
		    regents[i].mfGetid() != 1056 &&
		    regents[i].mfGetid() != 1999)
		{
#if defined (_DEBUG)
           SHORT oldRace = regents[i].mfGetRace();
		   SHORT oldClass1 = regents[i].mfGetClass1();
		   SHORT oldLevel1 = regents[i].mfGetLevel1();
		   SHORT oldClass2 = regents[i].mfGetClass2();
		   SHORT oldLevel2 = regents[i].mfGetLevel2();
#endif
		   RACE_INFO::TYPE raceType = pPS->mfGetRace();
		   if (raceType != RACE_INFO::NOT_SET)
			   regents[i].mfSetRace((RACE_ENUM)raceType);

		   PLAYER_CLASS_INFO::TYPE classType = pPS->mfGetClassType (0);
		   if (classType != PLAYER_CLASS_INFO::NOT_SET)
		   {
			   regents[i].mfSetClass1(gaClassTranslate[classType]);
			   regents[i].mfSetLevel1(pPS->mfGetLevel (0));
		   }

		   if (pPS->mfGetClassType (1) != PLAYER_CLASS_INFO::NOT_SET)
		   {
			   regents[i].mfSetClass2(gaClassTranslate[pPS->mfGetClassType (1)]);
			   regents[i].mfSetLevel2(pPS->mfGetLevel (1));
		   }

			// [d5-30-97 JPC] Change the map icon to match the character art.
			// (The character art can be changed by running the character
			// editor and saving a modified AVA file.)
			oldIcon = newIcon = (MAP_ICON) regents[i].mfGeticon ();
			for (icref = 0; icref < gcCrossRefs; icref++)
			{
				if (gIconCrossRef[icref].ttype == pPS->mfGetType ())
				{
					newIcon = gIconCrossRef[icref].icon;
					break;
				}
			}
			// Only change the icons of the regents.
			if (oldIcon != newIcon 
			    && (oldIcon >= REG_LORDM1_UNIT_ICON 
			        && oldIcon <= REG_GLADY1_UNIT_ICON))
	        {
				regents[i].mfSeticon (newIcon);
			}
		}

		printf("Loaded id%d.ava, with handle %d, for character %d\n",
			regents[i].mfGetid(), playerstats[i], i);
        fflush(stdout);


		// [d5-05-97 JPC] Kludge implemented, see notes above.
		if (gBloodInfo.BL_strength >= 0)
		{
			regents[i].mfSetBL_grade    ((BLOOD_GRADE)gBloodInfo.BL_grade);
			regents[i].mfSetBL_deriv    ((BLOOD_DERIV)gBloodInfo.BL_deriv);
			regents[i].mfSetBL_strength (gBloodInfo.BL_strength);
		}
    }

#if 0	
	if (SaveAfter)
	{
		printf("Writing stats to single file.\n");
		StatWriteAll();
	}
#endif

//DEATH
//    for (i=1; i < CHARACTER_COUNT; i++)
//    {
//        SetLock(playerstats[i]);
//    }

    playerStatsInitted = TRUE;
    return;
}

/* ========================================================================
   Function    - StatReadAllFrom
   Description - Read in the PLAYER_STATS array from a stored game.
   Returns     -
   ======================================================================== */

void StatReadAllFrom(FILE *fp)
{
    LONG i, count;
    if (playerStatsInitted)
        PurgePS(CHARACTER_COUNT);

    playerstats[0] = fERROR;

    if (!fp || feof(fp)) /* no overall stats file; read from *.AVA/D files */
    {
        StatReadFromAVA(TRUE);
        return;
    }

    /* check file version: items version *and* characters version */

    fread( &count, sizeof(LONG), 1, fp);
//    if (count != MAX_ITEM_TYPES)  /* update in itemtype */
//    {
//        printf("Wrong items version. Re");  //loading from AVA... :)
//        StatReadFromAVA(TRUE);
//        return;
//    }

    fread( &count, sizeof(LONG), 1, fp);
    if (count != CHARACTER_COUNT - 1)
    {
        printf("Wrong characters version. Re");
        StatReadFromAVA(TRUE);
        return;
    }

    for (i=1; i < CHARACTER_COUNT; i++)
    {
        playerstats[i] = PLAYER_STATS::mfAllocateSpace(fERROR, regents[i].mfGetid());

        if (playerstats[i] < 0) /* failed in allocation; unload them all */
        {
            PurgePS(i);
            return;
        }

		DumbAutoLockPtr <PLAYER_STATS> const pPS(playerstats[i]);
        if (fread((PLAYER_STATS *) pPS, sizeof(PLAYER_STATS), 1, fp) < 1)
        {
            PurgePS(i+1);
            return;
        }

        // at this point, the hThis has been toasted... restore it.
        pPS->hThis = playerstats[i];

		pPS->hAvatar = fERROR;  // always loaded at map level... no avatar
		pPS->Inventory.mfInit(playerstats[i],regents[i].mfGetid());
		pPS->StatsMod.mfClearStatMods();
        pPS->Inventory.mfLoadFrom(fp);

   		run_timers();
	}
//DEATH
//    for (i=1; i < CHARACTER_COUNT; i++)
//    {
//        SetLock(playerstats[i]);
//    }

    playerStatsInitted = TRUE;

}

/* ========================================================================
   Function    - StatReadAll
   Description - Read stats from a temporary save file.
   Returns     -
   ======================================================================== */

void StatReadAll()
{
    FILE *fp;
    char filename[128];

    strcpy(filename, "playstat.sav");

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // GEH fix a bug
    // GEH TEMP TEMP
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// fp = FileOpen(filename, "rb");

    fp = 0;
    StatReadAllFrom(fp);
    if (fp)
        FileClose(fp);
}


/* ========================================================================
   Function    - StatWriteAllTo
   Description - Write a binary PLAYER_STATS array
   Returns     -
   ======================================================================== */

void StatWriteAllTo(FILE *fp)
{
    LONG i, count;

    if (! playerStatsInitted  ||  ! fp)
        return;

    count = MAX_ITEM_TYPES;
    fwrite( &count, sizeof(LONG), 1, fp);

    count = CHARACTER_COUNT - 1;
    fwrite( &count, sizeof(LONG), 1, fp);
    for (i=1; i < CHARACTER_COUNT; i++)
    {
        DumbAutoLockPtr < PLAYER_STATS> const pPS(playerstats[i]);
        fwrite((PLAYER_STATS *) pPS, sizeof(PLAYER_STATS), 1, fp);
        pPS->Inventory.mfSaveTo(fp);
		run_timers();
    }
}

/* ========================================================================
   Function    - StatWriteAll
   Description - Write a temporary PLAYER_STATS array save file
   Returns     -
   ======================================================================== */

void StatWriteAll()
{
    FILE *fp;
    char filename[128];

    if (! playerStatsInitted )
        return;

    strcpy(filename, "playstat.sav");
    fp = FileOpen(filename, "wb");
    if (fp) {
	    StatWriteAllTo(fp);
	    fflush(fp);
	    FileClose(fp);
    }
}




#if 0
LONG PLAYER_STATS::mfWriteTextData(FILE *fp)
{
	LONG Result;
	
	fprintf(fp, "%ld\n", (LONG)ThingType);
	fprintf(fp, "%ld\n", StructVersion);		/* PLAYER_STATS::VERSION  */
	Ability.mfWriteData(fp);					/* ABILITY	*/
	fprintf(fp, "%ld\n", (LONG)Race);			/* PLAYER_RACE_TYPE */	
	fprintf(fp, "%ld\n", Alignment);			/* ALIGNMENT_TYPE */	
	fprintf(fp, "%hd\n", HitPts);				/* LONG */	
	fprintf(fp, "%ld\n", (LONG)sbArmorClass);  	/* LONG */	
	fprintf(fp, "%ld\n", (LONG)Inventory.mfGetCurrentArmor());		/* ARMOR_TYPE */	
	fprintf(fp, "%ld\n", (LONG)Inventory.mfGetCurrentWeapon());		/* ARMOR_TYPE */	
	fprintf(fp, "%d\n",  sMorale);				/* SHORT	*/
	//fprintf(fp, "%s\n",  Task);
	//fprintf(fp, "%ld\n", (LONG)ShieldType);		/* SHIELD_TYPE */	
	//fprintf(fp, "%d\n",  IdHeir);				/* SHORT	*/
	//fprintf(fp, "%ld\n", Age);					/* LONG */	
	//fprintf(fp, "%ld\n", Height);				/* LONG */	
	//fprintf(fp, "%ld\n", Weight);				/* LONG */	
	
	//GEH NOTE: these three pointers to lists need special treatment
	//fprintf(fp, "%ld\n", PlayerClassList);	/* PLAYER_CLASS_LIST	*/
	//fprintf(fp, "%ld\n", CurrentWeapons);		/* PTR_WEAPON_LIST */	
	//fprintf(fp, "%ld\n", pSpellBook);			/* PTR_SPELL_LIST */	
	//fprintf(fp, "%ld\n", pSpellSchools);		/* PTR_SPELL_SCHOOL_LIST	*/
	//fprintf(fp, "%ld\n", BloodInfo);			/* PLAYER_BLOOD_INFO	*/
	return Result; // bogus
}
#endif

/* ========================================================================
   Function    - LoadStats
   Description - Load the stats for this avatar from a .ava file
   Returns     - handle to stats structure
   ======================================================================== */
SHORT LoadStats(LONG tID,SHORT hAvatar)
{
	char	szFileName[40];
	SHORT	hPlayerStats = fERROR;
	SHORT	hRPlayerStats = fERROR;
	
	/* theoretically, if we've loaded all the stats, we never have
	   to call this function for regent/lieutenant characters again.
	   Therefore, we simply search the regents[] array until we find
	   the regent with that ID, and return its playerstats[] entry,
	   which is exactly the hPlayerStats we want!  */

    /* note: this makes it rather crucial that character tID's be
       unique.  Really, anything that knows it's a regent/lieutenant
       shouldn't be calling this function... (hint hint)  */
	
	if (playerStatsInitted && tID >= 1000 && tID < 3000)
	{
	    LONG i;
	    PTR_PLAYER_STATS pPS;
		CAvatar* pAvatar;
	
		for (i=1; i < CHARACTER_COUNT; i++)
		{
			if (regents[i].mfGetid() == tID)
			{
				pPS = (PTR_PLAYER_STATS) BLKPTR(playerstats[i]);
				if (hAvatar != fERROR)
				{
					// Don't overwrite good data with bad. Delete Avatar
					// will clear this handle when it goes away.
					pPS->hAvatar = hAvatar;
					pAvatar = (CAvatar *) BLKPTR (hAvatar);
					pAvatar->attrib.RuntimeCreated = FALSE;
				}
				return playerstats[i];
			}
		}
		printf("LoadStats: tID=%d seems to be the ID of a PC/NPC, "
				"but doesn't match any I know of.\n", tID);
	}
	
	sprintf(szFileName,"scenes\\id%ld.avd", tID);

	// We may at some time in the future want to check the result of the freads!
	
	hRPlayerStats = GetResourceStd(szFileName,(BOOL)hAvatar); //bool?
	
	if ( fStartMultiGame )
	{
		// we must reload all these puppies from CDROM in the multiplayer
		// version.  Dispose of this on and try again.
		DisposBlock(hRPlayerStats);
		hRPlayerStats = GetResourceStd(szFileName,(BOOL)hAvatar);
	}
		
	if (hRPlayerStats < 0 && tID > 1000 && tID < 3000)
	{
#if defined(_DEBUG)
        char buffer[100];
        sprintf(buffer, "Can't load resource %s", szFileName);
        fatal_error(buffer);
#else
        printf("Failed to load %s; trying default...\n", szFileName);
#endif
	    hRPlayerStats = GetResourceStd("scenes\\id1999.avd",(BOOL)hAvatar);
	}
	
	if (hRPlayerStats > fERROR)
	{
		// Allocate another one and copy this one into it.
		hPlayerStats = PLAYER_STATS::mfAllocateSpace(hAvatar,tID);
		
		DumbAutoLockPtr < PLAYER_STATS> const pPlayerStats(hPlayerStats);
		DumbAutoLockPtr < PLAYER_STATS> const pRPlayerStats(hRPlayerStats);
		// Structure copy only.
		*pPlayerStats = *pRPlayerStats;
		
		pPlayerStats->Inventory.mfInit(hPlayerStats,tID);
		pPlayerStats->StatsMod.mfClearStatMods();
		// hack -- Gorgon needs magic resistance
		if ((tID == 130 || tID == 143) &&
			pPlayerStats->StatsMod.mfGetMod(STATSMOD::MAGIC_RESIST_MOD) == 0)
		{
			pPlayerStats->StatsMod.mfModifyStats(STATSMOD::MAGIC_RESIST_MOD, 95);
		}
		
		pPlayerStats->Inventory.ReadINV(hPlayerStats, tID);
		
		pPlayerStats->hAvatar = hAvatar;
		if(hAvatar != fERROR)
		{
			CAvatar *pAvatar = (CAvatar *) BLKPTR (hAvatar);
			pAvatar->attrib.RuntimeCreated = TRUE;
		}
		
		SetPurge(hRPlayerStats);
		hRPlayerStats = fERROR;
	}
	
#if 0
	FILE	*pFileRead;
	
	pFileRead = FileOpen(szFileName,"rb");
	if (pFileRead != 0)
	{
		// Version'd files a production and are binary
		hPlayerStats = PLAYER_STATS::mfAllocateSpace(hAvatar);
		
		if (hPlayerStats != fERROR)
		{
			DumbAutoLockPtr <PLAYER_STATS> const pPlayerStats(hPlayerStats);
			pPlayerStats->mfReadData(pFileRead);
		}
		FileClose(pFileRead);
	}
	else
	{
		sprintf(szFileName,"scenes\\id%ld.ava", tID);
		pFileRead = FileOpen(szFileName,"r");
		if (pFileRead == NULL)
		{
			printf("WARNING! Unable to open file %s\n.", szFileName);
			return fERROR;
		}
		
		// read text file
		hPlayerStats = PLAYER_STATS::mfAllocateSpace(hAvatar);
		if (hPlayerStats != fERROR)
		{
			DumbAutoLockPtr<PLAYER_STATS> pPlayerStats(hPlayerStats);
			pPlayerStats->mfReadTextData(pFileRead);
		}

		FileClose(pFileRead);		
	}
		
#endif
	
	return hPlayerStats;
}

/* ========================================================================
   Function    - LoadAVD
   Description - Load the stats from a single binary lump file
                 // GEH First tries to load the .AVD file then if that fails,
                 loads the data from the .AVA file.
   Returns     -
   ======================================================================== */

SHORT LoadAVD (
	CSTRPTR szFileName,
	BOOL fhAvatar,
	BOOL /* fLockRes */,
	BOOL /* fRotated */,
	LONG /* iResFileSlot */)
{
	SHORT hPlayerStats = fERROR;
	FILE * pFileRead;
	FILE_MODE OldFileMode;
	
	// in the multiplayer game, we don't want any altered avatar file
	#ifndef _DEBUG
	if ( fStartMultiGame )
		OldFileMode = SetFileMode( FILEOPEN_CDROM_ONLY );
	#endif
	
	//GEH 9/97 We will never be using this feature and it is slowing
	//down the game.  Bye, Bye for now
	//pFileRead = FileOpen(szFileName,"rb");
	//if (pFileRead != 0)
	//{
	//	// Version'd files a production and are binary
	//	hPlayerStats = PLAYER_STATS::mfAllocateSpace((SHORT)fhAvatar,fERROR);
	//	if (hPlayerStats != fERROR)
	//	{
	//		DumbAutoLockPtr <PLAYER_STATS> const pPlayerStats(hPlayerStats);
	//		pPlayerStats->mfReadData(pFileRead);
	//	}
	//	FileClose(pFileRead);
	//}
	//else
	{
		// now we always stamp an A at the end of the file name
		char cpTextFileName[80];
		
		strcpy(cpTextFileName, szFileName);
		cpTextFileName[strlen(cpTextFileName) - 1] = 'A';
		
		pFileRead = FileOpen(cpTextFileName,"r");
		if (pFileRead == NULL)
		{
#if defined(_DEBUG)
            char buffer[100];
            sprintf(buffer, "Can't load file %s", cpTextFileName);			
            fatal_error(buffer);
#else			
			printf("WARNING! Unable to open file %s.\n", cpTextFileName);
#endif
			goto Error;
		}
		
		// read text file
		hPlayerStats = PLAYER_STATS::mfAllocateSpace((SHORT)fhAvatar,fERROR);
		if (hPlayerStats != fERROR)
		{
			DumbAutoLockPtr <PLAYER_STATS> pPlayerStats(hPlayerStats);
			pPlayerStats->mfReadTextData(pFileRead);
		}

		FileClose(pFileRead);		
	}
	
	// in the multiplayer game, reset file mode
	#ifndef _DEBUG
	if ( fStartMultiGame )
		SetFileMode( OldFileMode );
	#endif
	
	return hPlayerStats;
	
Error:
	return fERROR;
}
	
SHORT const PLAYER_STATS::mfCanMemorize(
	SPELL_INFO::CASTERTYPE type, SPELL_INFO::LEVEL level) const
{
	LONG i;
	SHORT max_spells = 0;
	
    for (i=0; i < MAX_PLAYER_CLASSTYPES; i++)
    {
        if (mfGetClassType(i) == PLAYER_CLASS_INFO::NOT_SET)
            break;

		SHORT spells_this_class =
			num_spells_of(type, level, mfGetClassType(i), mfGetLevel(i));

		if (mfGetClassType(i) == PLAYER_CLASS_INFO::PRIEST
			&& type == SPELL_INFO::PRIEST && spells_this_class > 0)
		{
			spells_this_class +=
				WISDOM::mfGetPriestBonus(mfGetWis(), level);
		}

		if (type == SPELL_INFO::WIZARD)
		{
			spells_this_class *=
				(1 + StatsMod.mfGetMod(STATSMOD::SPELLMULT_MOD));
		}

        if (spells_this_class > max_spells)
        {
            max_spells = spells_this_class;
	    }
    }

    return (max_spells);
}
	
SHORT const PLAYER_STATS::mfMemorized(
	SPELL_INFO::CASTERTYPE type, SPELL_INFO::LEVEL level)
{
	InventoryItor itor(Inventory);
	SHORT cur_num_memorized = 0;

	for (itor.mfAdvance(IC_SPELL);
		itor != Inventory.end();
		itor.mfAdvanceToNext(IC_SPELL) )
	{
        ITEMTYPE const 							item	= (*itor)->mfType();
    	SPELL_INFO::TYPE const					index	= (SPELL_INFO::TYPE) ItemTypes[item].mfGetExtendedData();
        SPELL_INFO::SPELL_ITEM const & const	spell	= SPELL_INFO::mfGetSpell(index);

        if (spell.Type == type && spell.Level == level)
        {
            cur_num_memorized += (*itor)->mfHowMany();
        }
    }

    return cur_num_memorized;
}
	
/* ========================================================================
   Function    - mfCanIMemorizeThisSpell
   Description - Test whether a character can memorize this spell.
   Returns     -
   ======================================================================== */

BOOL const PLAYER_STATS::mfCanIMemorizeThisSpell(ITEMTYPE const NewSpell)
{
    BOOL Result = 0;
	
    SPELL_INFO::TYPE const 					index	= (SPELL_INFO::TYPE) ItemTypes[NewSpell].mfGetExtendedData();
    SPELL_INFO::SPELL_ITEM const & const	spell	= SPELL_INFO::mfGetSpell(index);
    SPELL_INFO::CASTERTYPE const 			type	= spell.Type;
    SPELL_INFO::LEVEL const 				level	= spell.Level;

    if (mfMemorized(type, level) < mfCanMemorize(type, level))
    {
        Result = TRUE;
	}
	
	return Result;
}

/* ========================================================================
   Function    - mfMemorizeAllInvSpells
   Description - Learn all the spells you can from your inventory.
   				 Called by NPC's when loaded from the wad.
   Returns     -
   ======================================================================== */

void PLAYER_STATS::mfMemorizeAllInvSpells()
{
	InventoryItor itor(Inventory);
	
	for (itor.mfAdvance(IC_SPELL);
	     itor != Inventory.end();
	     itor.mfAdvanceToNext(IC_SPELL))
	{
		ITEMTYPE const itemType = (*itor)->mfType();
		LONG infiniteLoop;
		
		// memorize as many charges as you can.
		for (infiniteLoop = 0;
		     mfCanIMemorizeThisSpell(itemType) && infiniteLoop < 99;
		     infiniteLoop++)
	     {
	     	(*itor)->mfGainOne();
	     }
	}
}


/* ========================================================================
   Function    - IsAvatarDead
   Description - C callable function to determine whether or not an Avatar
                 (passed in by a thingtype) is dead or not.
   Returns     - TRUE if the avatar is dead
   ======================================================================== */
extern "C"
BOOL IsAvatarDead(THINGTYPE t)
{
//WRC, this function is going to slow the map down a lot...
//there must be a faster way to turn a THINGTYPE into a CAvatar...

	if (SCENE_MGR::hCurrentScene==fERROR)
		return TRUE;

	SCENE const * pCurrentScene=(SCENE const *) BLKPTR(SCENE_MGR::hCurrentScene);

	for (LONG i=0;i<MAX_AVATARS;i++)
	{
		if (pCurrentScene->Avatars[i] != fERROR)
		{
			CAvatar* pAvaTesting=(CAvatar*)BLKPTR(pCurrentScene->Avatars[i]);
			if (pAvaTesting->ThingType==t)
			{
				if(pAvaTesting->hPlayerStats == fERROR)
					return FALSE;

				PLAYER_STATS const * pAvatarStats=(PLAYER_STATS const *)BLKPTR(pAvaTesting->hPlayerStats);
				
				if (pAvatarStats->mfGetCurHitPoints()<=0)
					return TRUE;
				return FALSE;
			}
		}
	}
	return TRUE;
}

/* ========================================================================
   Function    - GetThingTypeFromID
   Description - Without making a complete copy of the stats,
                 just get the thingtype.
   Returns     -
   ======================================================================== */

THINGTYPE const GetThingTypeFromID(LONG const tID)
{
	THINGTYPE Result = NO_THING;
	
	if (playerStatsInitted && tID >= 1000 && tID < 3000)
	{
	    LONG i;
		for (i=1; i < CHARACTER_COUNT; i++)
		{
			if (regents[i].mfGetid() == tID)
			{
				PLAYER_STATS const * const pPS = (PLAYER_STATS const * const) BLKPTR(playerstats[i]);
				Result = pPS->mfGetType();
				break;
			}
		}
	}
	else
	{
		char cpTextFileName[80];
		
		sprintf(cpTextFileName,"scenes\\id%ld.avd", tID);
		SHORT hPlayerStats = GetResourceStd(cpTextFileName,(BOOL)fERROR); //bool?
		
		if ( fStartMultiGame )
		{
			// we must reload all these puppies from CDROM in the multiplayer
			// version.  Dispose of this on and try again.
			DisposBlock(hPlayerStats);
			hPlayerStats = GetResourceStd(cpTextFileName,(BOOL)fERROR);
		}
		
		if (hPlayerStats != fERROR)
		{
			PLAYER_STATS const * const pPS = (PLAYER_STATS const * const) BLKPTR(hPlayerStats);
			
			Result = pPS->mfGetType();
			SetPurge(hPlayerStats);
		}
	}
	
	return Result;
}
// Release any allocated memory.
void PLAYER_STATS::mfDelete()
{
	ItemEffects::DeactivateAvatarsItems(hThis);
	if (hAvatar != fERROR)
	{
		CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(hAvatar);
		if (pAvatar->attrib.RuntimeCreated)
		{
			Inventory.mfDeinit();
			// Horked value for hThis! DisposBlock(hThis);
			hThis = fERROR;			// Just in case.
		}
	}
	hAvatar = fERROR;
}

