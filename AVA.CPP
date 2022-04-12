/* ========================================================================
   Copyright (c) 1997	Synergistic Software
   All Rights Reserved
   ========================================================================
	Character editor for Birthright--support file to load and save AVA files.
   Filename: AVA.CPP
   Author:	 John Conley
   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WINDOWS
#include <windows.h>
#include <windowsx.h>
#endif

#include "SYSTEM.H"
#include "ENGINE.H"

#include "AVA.HXX"
#include "BLOOD.HXX"

#ifdef _WINDOWS								// Windows is required for these functions


/* ========================================================================
   Function    - WarnMessage
   Description - Put a message in a message box and wait for a key.
   Returns     - Nothing
	Note:			- Does not abort.
   ======================================================================== */
static void WarnMessage (PSTR szMessage)
{
	MessageBox (NULL, szMessage, "Warning", MB_OK);
}



/* ========================================================================
   Function    - mfReadTextData
   Description - Read a .ava file and fill in the values
   			 	 compute any values necessary from the .ava data.
   Returns     - Status of last file read().
	Note:			- Boldly lifted from LONG PLAYER_STATS::mfReadTextData
   ======================================================================== */

LONG AVA_FILE::mfReadTextData (FILE *fp)
{
	LONG			Result;
	LONG			lTemp;
	LONG			lTemp2;
	LONG			lTemp3;
	LONG			lTemp4;
	LONG			lTemp5;
	LONG			lTemp6;
	char			cpBuffer[126];
	
	
	if (EOF != GetNextLine(fp, cpBuffer, sizeof(cpBuffer)))
	{
		sscanf(cpBuffer, "%ld", &fFileVersion);
	}
	else
	{
		WarnMessage ("WARNING: No File version in AVA file.\n");
		goto Leave_fn;
	}
	
	if (EOF != GetNextLine(fp, cpBuffer, sizeof(cpBuffer)))
	{
		sscanf(cpBuffer, "%ld", &lTemp);
		fThingType = (THINGTYPE)lTemp;
	}
	else
	{
		WarnMessage ("WARNING: No ThingType Id in AVA file.\n");
		fThingType = NO_THING;
		goto Leave_fn;
	}
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing  AVA Version number.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &fStatsVersion);		
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing AVA strength ability info.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &fStrength);
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing AVA dexterity ability info.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &fDexterity);
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing AVA constitution ability info.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &fConstitution);
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing AVA intelligence ability info.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &fIntelligence);
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing AVA wisdom ability info.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &fWisdom);
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing AVA charisma ability info.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &fCharisma);
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing AVA player race info.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &fRace);
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing player name index.\n");
		goto Leave_fn;
	}
   // [d4-30-97 JPC] Changed to new style: player name is a string in
	// the AVA file, not merely a pointer.
	// sscanf(cpBuffer, "%ld", &fName);
	// PLAYER_STATS::NameClass.fName =(PLAYER_NAME::NAME) lTemp;
	// strncpy (szName, cpBuffer, MAX_NAME_LEN);
	mfSetName (cpBuffer);

	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing player title.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &fTitle);
	// PLAYER_STATS::TitleClass.fTitle =(PLAYER_TITLE::TITLE) lTemp;

	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing player description.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &fDescription);
	// PLAYER_STATS::DesClass.fDescription =(PLAYER_DESCRIPTION::DES) lTemp;

	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing player alignment data.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &fAlignment);			/* ALIGNMENT_TYPE */	
	// Alignment = (ALIGNMENT_INFO::TYPE) lTemp;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing player hit point data in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &fHitPoints);				/* LONG */	
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing Armor Class information in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &fArmorClass);			/* LONG */	
	// sbArmorClass = (SBYTE) lTemp;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing Armor type information in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &fArmorType);			/* ARMOR_TYPE */	
	// DefaultArmor = (ARMOR_INFO::TYPE) lTemp;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing Weapon type information in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &fWeapon);			/* WEAPON_TYPE */	
	// DefaultWeapon = (WEAPON_INFO::TYPE) lTemp;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing Morale data in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld",  &fMorale);
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing Full HitPt data in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &fTotalHitPoints);
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing Thac0 data in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &fTHAC0);
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing Damage Dice data in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "(%ld %ld %ld) (%ld %ld %ld)",
	                   &lTemp, &lTemp2, &lTemp3, &lTemp4, &lTemp5, &lTemp6);
	fDamageDice[0] = lTemp;
	fDamageDice[1] = lTemp2;
	fDamageDice[2] = lTemp3;
	fDamageDice[3] = lTemp4;
	fDamageDice[4] = lTemp5;
	fDamageDice[5] = lTemp6;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing Hit Dice data in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "(%ld %ld %ld)", &lTemp, &lTemp2, &lTemp3);
	fHitDice[0] = lTemp;
	fHitDice[1] = lTemp2;
	fHitDice[2] = lTemp3;
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing Experience Pts data in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld", &fExperience);
	
	// Result = PlayerClassList.mfReadTextData (fp);
	
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing class 1 data in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld %ld %ld", &lTemp, &lTemp2, &lTemp3);
	fClass1[0] = lTemp;
	fClass1[1] = lTemp2;
	fClass1[2] = lTemp3;

	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("WARNING: Missing class 2 data in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer, "%ld %ld %ld", &lTemp, &lTemp2, &lTemp3);
	fClass2[0] = lTemp;
	fClass2[1] = lTemp2;
	fClass2[2] = lTemp3;

	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF == Result)
	{
		WarnMessage ("Warning: Missing number of Attacks in AVA file.\n");
		goto Leave_fn;
	}
	sscanf(cpBuffer,"%ld", &fAttacksPerRound);
	// NumberOfAttacks = (SBYTE) lTemp;
	
	// Now Calculate the other fields as necessary.
	// if (fHitPoints == 0)
	// {
	// 	if (HitDice.sbNumberOfDice != 0)
	// 	{
	// 		fHitPoints = RollDice(&HitDice);
	// 	}
	// 	else
	// 	{
	// 		fHitPoints = PlayerClassList.mfRollHitPts();
	// 	}
	// 	
	// }
	
	// if (fTotalHitPoints == 0)
	// {
	// 	fTotalHitPoints = fHitPoints;
	// }
	
	// if (fArmorType != ARMOR_INFO::NO_ARMOR)
	// {
	// 	sbArmorClass = ARMOR_INFO::mfGetArmorClass(fArmorType);
	// }
	
	// if (fTHAC0 == 0)
	// {
	// 	fTHAC0 = PlayerClassList.mfGetBestThac0();
	// }

	// Get bloodline information if present.  (It is not present in the
	// "official" AVA files delivered with Birthright I.)
	fBloodlineGrade = -1;					// [d5-07-97 JPC] -1 means "get info
	fBloodlineDerivation = -1; 			// from regents array"
	fBloodlineStrength = -1;
	Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
	if (EOF != Result)
	{
		sscanf(cpBuffer,"%ld", &fBloodlineGrade);
		Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
		if (EOF != Result)
		{
			sscanf(cpBuffer,"%ld", &fBloodlineDerivation);
			Result = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
			if (EOF != Result)
			{
				sscanf(cpBuffer,"%ld", &fBloodlineStrength);
			}
		}
	}
	
Leave_fn:
	return Result;
}

/* ========================================================================
   Function    - mfWriteTextData
   Description - Write to an .ava file
   Returns     - Status of last fwrite().
   ======================================================================== */

LONG AVA_FILE::mfWriteTextData (FILE *fp)
{
	fprintf (fp, "# File Version\n");
	fprintf (fp, "%d\n", fFileVersion);

	fprintf (fp, "# Thing type\n");
	fprintf (fp, "%d\n", fThingType);

	fprintf (fp, "# PLAYER_STATS class version\n");
	fprintf (fp, "%d\n", fStatsVersion);

	fprintf (fp, "# Strength\n");
	fprintf (fp, "%d\n", fStrength);

	fprintf (fp, "# Dexterity\n");
	fprintf (fp, "%d\n", fDexterity);

	fprintf (fp, "# Constitution\n");
	fprintf (fp, "%d\n", fConstitution);

	fprintf (fp, "# Intelligence\n");
	fprintf (fp, "%d\n", fIntelligence);

	fprintf (fp, "# Wisdom\n");
	fprintf (fp, "%d\n", fWisdom);

	fprintf (fp, "# Charisma\n");
	fprintf (fp, "%d\n", fCharisma);

	fprintf (fp, "# Race\n");
	fprintf (fp, "%d\n", fRace);

	fprintf (fp, "# Name\n");
	fprintf (fp, "%s\n", mfGetName ());

	fprintf (fp, "# Title\n");
	fprintf (fp, "%d\n", fTitle);

	fprintf (fp, "# Description\n");
	fprintf (fp, "%d\n", fDescription);

	fprintf (fp, "# Alignment\n");
	fprintf (fp, "%d\n", fAlignment);

	fprintf (fp, "# HitPoints\n");
	fprintf (fp, "%d\n", fHitPoints);

	fprintf (fp, "# ArmorClass\n");
	fprintf (fp, "%d\n", fArmorClass);

	fprintf (fp, "# ArmorType\n");
	fprintf (fp, "%d\n", fArmorType);

	fprintf (fp, "# Weapon\n");
	fprintf (fp, "%d\n", fWeapon);

	fprintf (fp, "# Morale\n");
	fprintf (fp, "%d\n", fMorale);

	fprintf (fp, "# TotalHitPoints\n");
	fprintf (fp, "%d\n", fTotalHitPoints);

	fprintf (fp, "# THAC0\n");
	fprintf (fp, "%d\n", fTHAC0);

	fprintf (fp, "# Damage Dice\n");
	fprintf (fp, "(%d %d %d) (%d %d %d)\n",
		fDamageDice[0],
		fDamageDice[1],
		fDamageDice[2],
		fDamageDice[3],
		fDamageDice[4],
		fDamageDice[5]);

	fprintf (fp, "# Hit Dice\n");
	fprintf (fp, "(%d %d %d)\n",
		fHitDice[0],
		fHitDice[1],
		fHitDice[2]);

	fprintf (fp, "# Experience\n");
	fprintf (fp, "%d\n", fExperience);

	fprintf (fp, "# Player class information\n");
	fprintf (fp, "%d %d %d\n",
		fClass1[0],
		fClass1[1],
		fClass1[2]);
	fprintf (fp, "%d %d %d\n",
		fClass2[0],
		fClass2[1],
		fClass2[2]);

	fprintf (fp, "# AttacksPerRound\n");
	fprintf (fp, "%d\n", fAttacksPerRound);

	fprintf (fp, "# BloodlineGrade\n");
	fprintf (fp, "%d\n", fBloodlineGrade);

	fprintf (fp, "# BloodlineDerivation\n");
	fprintf (fp, "%d\n", fBloodlineDerivation);

	fprintf (fp, "# BloodlineStrength\n");
	fprintf (fp, "%d\n", fBloodlineStrength);

	return 0;
}


#endif
