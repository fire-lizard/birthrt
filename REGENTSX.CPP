/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: REGENTS.cpp (part of GAMEMAP.cpp)
   Author:   Alan Clark
   ========================================================================
   Contains the following general functions:
   ======================================================================= */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "typedefs.h"

#include "units.hxx"
#include "regents.hxx"
#include "gamemap.hxx"
#include "vector.hxx"
#include "realm.hxx"
#include "playstat.hxx"

// [d4-23-97 JPC] Removed the "name" field of regents struct.
DEFINE_VECTOR_DATA(REGENT,regents) = {
/* 00 */	{/*"",*/ 0,0,0,0,0,0,0,0,0,0},
/* 01 */	{/*"Liliene Swordwraith",*/	REALM::AERENWE,	ANUIREAN,	FIGHTER,	10,	NO_CLASS,	0,		MAJOR,	ANDUIRAS,	37,	235,	1028,	REG_LORDF3_UNIT_ICON},
/* 02 */	{/*"Cole Alwier",*/			REALM::AERENWE,	HALFELF,		FIGHTER,	4,	WIZARD,		4,		MINOR,	REYNIR,		15,	236,	1029,	REG_ELORD1_UNIT_ICON},
/* 03 */	{/*"Carilon Alam",*/			REALM::ALAMIE,		ANUIREAN,	FIGHTER,	2,	NO_CLASS,	0,		MAJOR,	BASAIA,		46,	237,	1007,	REG_LORDM2_UNIT_ICON},
/* 04 */	{/*"Dierdren Alam",*/		REALM::ALAMIE,		ANUIREAN,	WIZARD,		6,	NO_CLASS,	0,		MAJOR,	BASAIA,		46,	238,	1008,	REG_WIZDF1_UNIT_ICON},
/* 05 */	{/*"Caliedhe Dosiere",*/	REALM::ANUIRE,		ANUIREAN,	FIGHTER,	12,	WIZARD,		16,		GREAT,	ANDUIRAS,	64,	239,	1003,	REG_LORDM6_UNIT_ICON},
/* 06 */	{/*"unknown",*/				REALM::ANUIRE,		ANUIREAN,	FIGHTER,	6,	WIZARD,		8,		MAJOR,	ANDUIRAS,	32,	0,		1999,	A_EIN_UNIT_ICON},
/* 07 */	{/*"Darien Avan",*/			REALM::AVANIL,		ANUIREAN,	FIGHTER,	9,	NO_CLASS,	0,		GREAT,	ANDUIRAS,	70,	241,	1044,	REG_LORDM4_UNIT_ICON},
/* 08 */	{/*"Dheraene Bhailie",*/	REALM::AVANIL,		ANUIREAN,	THIEF,		7,	NO_CLASS,	0,		MAJOR,	ANDUIRAS,	34,	242,	1045,	REG_LORDF3_UNIT_ICON},
/* 09 */	{/*"Grimm Graybeard",*/		REALM::BARUKAZHIK,DWARF,		FIGHTER,	7,	PRIEST,		8,		MAJOR,	ANDUIRAS,	38,	243,	1017,	REG_DLORD1_UNIT_ICON},
/* 10 */	{/*"Diirk Watershold",*/	REALM::BARUKAZHIK,DWARF,		THIEF,		10,	NO_CLASS,	0,		MINOR,	ANDUIRAS,	29,	244,	1018,	REG_DLORD3_UNIT_ICON},
/* 11 */	{/*"Aeric Boeruine",*/		REALM::BOERUINE,	ANUIREAN,	FIGHTER,	12,	NO_CLASS,	0,		MAJOR,	ANDUIRAS,	60,	245,	1039,	REG_LORDM3_UNIT_ICON},
/* 12 */	{/*"Arlen Innes",*/			REALM::BOERUINE,	ANUIREAN,	WIZARD,		7,	NO_CLASS,	0,		MINOR,	VORYNN,		19,	246,	1002,	REG_WIZDM2_UNIT_ICON},
/* 13 */	{/*"Eriene Mierelen",*/		REALM::BROSENGAE,	HALFELF,		THIEF,		7,	NO_CLASS,	0,		MAJOR,	BASAIA,		41,	247,	1011,	REG_GILDF1_UNIT_ICON},
/* 14 */	{/*"Hyde Termonie",*/		REALM::BROSENGAE,	HALFELF,		PRIEST,		4,	NO_CLASS,	0,		MINOR,	BRENNA,		21,	248,	1012,	REG_PRSTM1_UNIT_ICON},
/* 15 */	{/*"Entier Gladanil",*/		REALM::CARIELE,	ANUIREAN,	FIGHTER,	5,	NO_CLASS,	0,		MINOR,	ANDUIRAS,	20,	249,	1035,	REG_LORDM4_UNIT_ICON},
/* 16 */	{/*"Mheallie Bireon",*/		REALM::CARIELE,	ANUIREAN,	THIEF,		8,	NO_CLASS,	0,		MAJOR,	ANDUIRAS,	37,	250,	1036,	REG_GILDF1_UNIT_ICON},
/* 17 */	{/*"Danita Chimaera",*/		REALM::CHIMAERON,	HALFELF,		WIZARD,		13,	NO_CLASS,	0,		MINOR,	AZRAI,		38,	251,	1043,	REG_WIZDF1_UNIT_ICON},
/* 18 */	{/*"Denerik",*/				REALM::CHIMAERON,	ANUIREAN,	THIEF,		6,	NO_CLASS,	0,		MINOR,	BRENNA,		29,	252,	1042,	REG_GILDM1_UNIT_ICON},
/* 19 */	{/*"Eluvie Cariele",*/		REALM::COERANYS,	ANUIREAN,	FIGHTER,	2,	NO_CLASS,	0,		MINOR,	MASELA,		23,	253,	1009,	REG_LORDF2_UNIT_ICON},
/* 20 */	{/*"Medhlorie Haensen",*/	REALM::COERANYS,	ANUIREAN,	PRIEST,		2,	NO_CLASS,	0,		MAJOR,	ANDUIRAS,	36,	254,	1010,	REG_WIZDF1_UNIT_ICON},
/* 21 */	{/*"Fhiele Dhoesone",*/		REALM::DHOESONE,	HALFELF,		THIEF,		6,	NO_CLASS,	0,		MAJOR,	REYNIR,		46,	255,	1013,	REG_LORDF1_UNIT_ICON},
/* 22 */	{/*"Clumine Dhoesone",*/	REALM::DHOESONE,	HALFELF,		WIZARD,		3,	NO_CLASS,	0,		MAJOR,	VORYNN,		25,	256,	1014,	REG_WIZDM1_UNIT_ICON},
/* 23 */	{/*"Heirl Diem",*/			REALM::DIEMED,		ANUIREAN,	FIGHTER,	7,	NO_CLASS,	0,		MAJOR,	BRENNA,		42,	257,	1023,	REG_LORDM7_UNIT_ICON},
/* 24 */	{/*"Lasica Diem",*/			REALM::DIEMED,		ANUIREAN,	WIZARD,		5,	NO_CLASS,	0,		MAJOR,	BRENNA,		42,	258,	1024,	REG_WIZDF1_UNIT_ICON},
/* 25 */	{/*"Assan ibn Daouta",*/	REALM::ELINIE,		KHINASI,		PALADIN,	10,	NO_CLASS,	0,		MAJOR,	BASAIA,		47,	259,	1005,	REG_LORDM5_UNIT_ICON},
/* 26 */	{/*"Kalilah bint Daouta",*/REALM::ELINIE,		KHINASI,		PALADIN,	5,	NO_CLASS,	0,		MAJOR,	BASAIA,		37,	260,	1006,	REG_LORDF1_UNIT_ICON},
/* 27 */	{/*"Guilder Kalien",*/		REALM::ENDIER,		HALFELF,		THIEF,		5,	NO_CLASS,	0,		MAJOR,	BRENNA,		30,	261,	1019,	REG_GILDM1_UNIT_ICON},
/* 28 */	{/*"Caine",*/					REALM::ENDIER,		ANUIREAN,	WIZARD,		10,	NO_CLASS,	0,		MAJOR,	VORYNN,		27,	262,	1020,	REG_WIZDM3_UNIT_ICON},
/* 29 */		{/*"The Wizard",*/			REALM::FIVEPEAKS,	ANUIREAN,	WIZARD,		10,	NO_CLASS,	0,		MINOR,	VORYNN,		45,	263,	1081,	REG_PRSTF1_UNIT_ICON},
/* 30 */		{/*"",*/ REALM::MIERES,		0,	0,	0,	0,	0,	0,	0,	0,	0, 1999, A_BOW_UNIT_ICON},
/* 31 */		{/*"Gavin Tael",*/			REALM::GHOERE,		ANUIREAN,	FIGHTER,	9,	NO_CLASS,	0,		MAJOR,	REYNIR,		49,	265,	1046,	REG_LORDM2_UNIT_ICON},
/* 32 */		{/*"The Sword Mage",*/		REALM::GHOERE,		VOS,		WIZARD,		10,	NO_CLASS,	0,		MAJOR,	VORYNN,		42,	266,	1047,	REG_WIZDM4_UNIT_ICON},
/* 33 */		{/*"The Gorgon",*/			REALM::GORGONSCROWN,AWNSHEGH,	FIGHTER,	25,	WIZARD,		16,		TRUEBL,	AZRAI,		100,267,	1049,	GORGON_UNIT_ICON},
/* 34 */		{/*"Kiras Earthcore",*/		REALM::GORGONSCROWN,DWARF,		FIGHTER,	9,	NO_CLASS,	0,		MAJOR,	AZRAI,		48,	268,	1050,	REG_DLORD2_UNIT_ICON},
/* 35 */		{/*"Rogr Aglondier",*/		REALM::ILIEN,		ANUIREAN,	WIZARD,		3,	NO_CLASS,	0,		MINOR,	MASELA,		21,	269,	1037,	REG_WIZDM2_UNIT_ICON},
/* 36 */		{/*"Alliene Aglondier",*/	REALM::ILIEN,		ANUIREAN,	PRIEST,		3,	NO_CLASS,	0,		MINOR,	MASELA,		20,	270,	1038,	REG_PRSTF1_UNIT_ICON},
/* 37 */		{/*"Razzik Fanggrabber",*/	REALM::MARKAZOR,	GOBLIN,		FIGHTER,	4,	NO_CLASS,	0,		TAINTED,AZRAI,		2,	271,	1053,	REG_GLADY1_UNIT_ICON},
/* 38 */		{/*"unknown",*/				REALM::MARKAZOR,	GOBLIN,		FIGHTER,	3,	NO_CLASS,	0,		TAINTED,AZRAI,		2,	0,		1999,	A_EIN_UNIT_ICON},
/* 39 */		{/*"Suris Enlien",*/			REALM::MEDOERE,	ANUIREAN,	PRIEST,		4,	NO_CLASS,	0,		MAJOR,	REYNIR,		32,	273,	1031,	REG_PRSTF1_UNIT_ICON},
/* 40 */		{/*"Kotrin Skirvin",*/		REALM::MEDOERE,	RJURIK,		FIGHTER,	3,	NO_CLASS,	0,		MINOR,	ANDUIRAS,	18,	274,	1032,	REG_LORDM3_UNIT_ICON},
/* 41 */		{/*"Daeric Mhoried",*/		REALM::MHORIED,	ANUIREAN,	RANGER,		7,	NO_CLASS,	0,		MAJOR,	ANDUIRAS,	46,	275,	1040,	REG_LORDM7_UNIT_ICON},
/* 42 */		{/*"Michael Mhoried",*/		REALM::MHORIED,	ANUIREAN,	FIGHTER,	4,	NO_CLASS,	0,		MAJOR,	ANDUIRAS,	46,	276,	1041,	REG_LORDM1_UNIT_ICON},
/* 43 */		{/*"",*/ REALM::MIERES,		0,	0,	0,	0,	0,	0,	0,	0,	0, 1999, A_BOW_UNIT_ICON},
/* 44 */		{/*"",*/ REALM::MIERES,		0,	0,	0,	0,	0,	0,	0,	0,	0, 1999, A_BOW_UNIT_ICON},
/* 45 */		{/*"",*/ REALM::MONSTERS,	0,	0,	0,	0,	0,	0,	0,	0,	0, 1999, A_BOW_UNIT_ICON},
/* 46 */		{/*"",*/ REALM::MONSTERS,	0,	0,	0,	0,	0,	0,	0,	0,	0, 1999, A_BOW_UNIT_ICON},
/* 47 */		{/*"Godar Thurinson",*/		REALM::MURKILAD,	DWARF,		FIGHTER,	6,	NO_CLASS,	0,		TAINTED,VORYNN,		5,	281,	1048,	REG_DLORD2_UNIT_ICON},
/* 48 */		{/*"unknown",*/				REALM::MURKILAD,	DWARF,		FIGHTER,	4,	NO_CLASS,	0,		TAINTED,VORYNN,		3,	0,		1999,	A_EIN_UNIT_ICON},
/* 49 */		{/*"Jaison Raenech",*/		REALM::OSOERDE,	ANUIREAN,	FIGHTER,	7,	NO_CLASS,	0,		MAJOR,	BRENNA,		36,	283,	1051,	REG_LORDM1_UNIT_ICON},
/* 50 */		{/*"Terence Gryphon",*/		REALM::OSOERDE,	ANUIREAN,	FIGHTER,	4,	NO_CLASS,	0,		MINOR,	ANDUIRAS,	17,	284,	1052,	REG_LORDM2_UNIT_ICON},
/* 51 */		{/*"Rhuobhe the Elf",*/		REALM::RHUOBHE,	AWNSHEGH,	FIGHTER,	16,	WIZARD,		15,		TRUEBL,	AZRAI,		95,	285,	1054,	RHUOBHE_UNIT_ICON},
/* 52 */		{/*"Nhoun",*/					REALM::RHUOBHE,	ELF,		FIGHTER,	5,	NO_CLASS,	0,		MAJOR,	REYNIR,		37,	286,	1055,	REG_ELORD2_UNIT_ICON},
/* 53 */		{/*"Marlae Roesone",*/		REALM::ROESONE,	ANUIREAN,	FIGHTER,	5,	NO_CLASS,	0,		MAJOR,	BRENNA,		24,	287,	1000,	REG_LORDF2_UNIT_ICON},
/* 54 */		{/*"Michael Agnelie",*/		REALM::ROESONE,	ANUIREAN,	THIEF,		3,	NO_CLASS,	0,		TAINTED,BRENNA,		10,	288,	1030,	REG_GILDM2_UNIT_ICON},
/* 55 */		{/*"Isaelie",*/				REALM::SIELWODE,	ELF,		WIZARD, 	13,	NO_CLASS,	0,		MAJOR,	VORYNN,		42,	289,	1025,	REG_ELADY1_UNIT_ICON},
/* 56 */		{/*"Corwin Rhysdiordan",*/	REALM::SIELWODE,	ELF,		RANGER, 	7,	NO_CLASS,	0,		MAJOR,	REYNIR,		31,	290,	1026,	REG_ELORD2_UNIT_ICON},
/* 57 */		{/*"The Spider",*/			REALM::SPIDERFELL,AWNSHEGH,	WIZARD,		13,	NO_CLASS,	0,		MAJOR,	AZRAI,		95,	291,	1056,	SPIDERKING_UNIT_ICON},
/* 58 */		{/*"unknown",*/				REALM::SPIDERFELL,GOBLIN,		FIGHTER,	5,	NO_CLASS,	0,		TAINTED,AZRAI,		1,	0,		1999,	A_EIN_UNIT_ICON},
/* 59 */		{/*"Harald Khorien",*/		REALM::TAEGHAS,	ANUIREAN,	WIZARD,		4,	NO_CLASS,	0,		MAJOR,	ANDUIRAS,	33,	293,	1021,	REG_WIZDM2_UNIT_ICON},
/* 60 */		{/*"Trevor Onwen",*/			REALM::TAEGHAS,	ANUIREAN,	FIGHTER,	3,	NO_CLASS,	0,		MINOR,	ANDUIRAS,	16,	294,	1022,	REG_LORDM1_UNIT_ICON},
/* 61 */		{/*"Thuriene Donalls",*/	REALM::TALINIE, 	ANUIREAN,	PRIEST, 	6,	NO_CLASS,	0,		MAJOR,	ANDUIRAS,	33,	295,	1033,	REG_LORDF2_UNIT_ICON},
/* 62 */		{/*"Torele Anviras",*/		REALM::TALINIE,	ANUIREAN,	WIZARD,		10,	NO_CLASS,	0,		MAJOR,	ANDUIRAS,	35,	296,	1034,	REG_WIZDM3_UNIT_ICON},
/* 63 */		{/*"Tie'skar Graecher",*/	REALM::THURAZOR,	GOBLIN,		FIGHTER,	7,	NO_CLASS,	0,		MINOR,	AZRAI,		24,	297,	1057,	REG_GLORD1_UNIT_ICON},
/* 64 */		{/*"unknown",*/				REALM::THURAZOR,	GOBLIN,		PRIEST,		6,	NO_CLASS,	0,		TAINTED,AZRAI,		10,	0,		1999,	A_EIN_UNIT_ICON},
/* 65 */		{/*"Fhileraene",*/			REALM::TUARHIEVEL,ELF,		FIGHTER,	7,	WIZARD,		7,		GREAT,	REYNIR,		55,	299,	1015,	REG_ELORD1_UNIT_ICON},
/* 66 */		{/*"Llytha Damaan",*/		REALM::TUARHIEVEL,ELF,		WIZARD,		6,	THIEF,		6,		GREAT,	VORYNN,		34,	300,	1016,	REG_ELADY1_UNIT_ICON},
/* 67 */		{/*"Laela Flaertes",*/		REALM::TUORNEN,	HALFELF,		BARD,		4,	NO_CLASS,	0,		MINOR,	BRENNA,		35,	301,	1027,	REG_ELADY1_UNIT_ICON},
/* 68 */		{/*"Braedonnal Tuare",*/	REALM::TUORNEN,	ELF,		FIGHTER,	7,	NO_CLASS,	0,		MAJOR,	BASAIA,		34,	302,	1004,	REG_ELORD2_UNIT_ICON},

			/* Recruitable Lt. NPC's, most with holdings */

/* 69 */  		{/*"Adaere Doneim",*/		REALM::N_IMPORTS_EXPORTS,						ANUIREAN,	THIEF,	5,		NO_CLASS,	0,	MINOR,	REYNIR,		17,	0,	1001,	REG_GILDM2_UNIT_ICON},
/* 70 */		{/*"High Mage Aelies",*/	REALM::HIGH_MAGE_AELIES,						HALFELF,		WIZARD,	16,	NO_CLASS,	0,	MAJOR,	VORYNN,		36,	0,	1058,	REG_WIZDM3_UNIT_ICON},
/* 71 */		{/*"Maricoere",*/				REALM::HAELYNS_AEGIS,							ANUIREAN,	PRIEST,	6,		NO_CLASS,	0,	MAJOR,	BRENNA,		18,	0,	1059,	REG_WIZDF1_UNIT_ICON},
/* 72 */		{/*"Anuvier Iniere",*/		REALM::AVANIL,										ANUIREAN,	THIEF,	2,		NO_CLASS,	0,	MINOR,	BRENNA,		15,	0,	1074,	REG_GILDM1_UNIT_ICON},
/* 73 */		{/*"Arien Borthein",*/		REALM::BOERUINE,									ANUIREAN,	THIEF,	3,		NO_CLASS,	0,	MINOR,	REYNIR,		14,	0,	1060,	REG_GILDM2_UNIT_ICON},
/* 74 */		{/*"el Hadid",*/				REALM::PORT_OF_CALL_EXCHANGE,					ANUIREAN,	THIEF,	3,		NO_CLASS,	0,	TAINTED,	BRENNA,		10,	0,	1061,	REG_LORDM5_UNIT_ICON},
/* 75 */		{/*"Elamien Lamier",*/		REALM::POINTS_EAST_TRADING_GUILD,			ANUIREAN,	THIEF,	2,		NO_CLASS,	0,	MINOR,	BRENNA,		25,	0,	1062,	REG_GILDF1_UNIT_ICON},
/* 76 */		{/*"The Eyeless One",*/		REALM::WIZARD_EYELESS_ONE,						ANUIREAN,	WIZARD,	9,		NO_CLASS,	0,	MAJOR,	VORYNN,		35,	0,	1063,	REG_WIZDM4_UNIT_ICON},
/* 77 */		{/*"Fhylie The Sword",*/	REALM::MILITANT_ORDER_OF_CUIRAECEN,			HALFELF,		PRIEST,	6,		NO_CLASS,	0,	MAJOR,	REYNIR,		17,	0,	1064,	REG_LORDF1_UNIT_ICON},
/* 78 */		{/*"Gaelin Thuried",*/		REALM::UPPER_ANUIRE_TRADERS,					ANUIREAN,	THIEF,	4,		NO_CLASS,	0,	TAINTED,	BRENNA,		12,	0,	1065,	REG_GILDM1_UNIT_ICON},
/* 79 */		{/*"Ghorien Hirele",*/		REALM::GHOERE,										ANUIREAN,	THIEF,	5,		NO_CLASS,	0,	MINOR,	ANDUIRAS,	20,	0,	1066,	REG_GILDM2_UNIT_ICON},
/* 80 */		{/*"Hermedhie",*/				REALM::WIZARD_HERMEDHIE,						ANUIREAN,	WIZARD,	7,		NO_CLASS,	0,	MAJOR,	VORYNN,		24,	0,	1067,	REG_WIZDF2_UNIT_ICON},
/* 81 */		{/*"Hubaere Armiendin",*/	REALM::IMPREGNABLE_HEART_OF_HAELYN,			ANUIREAN,	PRIEST,	11,	NO_CLASS,	0,	MAJOR,	ANDUIRAS,	27,	0,	1068,	REG_PRSTM2_UNIT_ICON},
/* 82 */		{/*"Larra Nielems",*/		REALM::N_REFORMED_CHURCH_OF_SARIMIE,		ANUIREAN,	PRIEST,	8,		NO_CLASS,	0,	MAJOR,	BRENNA,		32,	0,	1069,	REG_PRSTF1_UNIT_ICON},
/* 83 */		{/*"Lavalan Briesen",*/		REALM::ORTHODOX_IMPERIAL_TEMPLE_OF_HAELYN,	ANUIREAN,	PRIEST,	5,	NO_CLASS,	0,	MAJOR,	ANDUIRAS,	20,	0,	1070,	REG_PRSTM1_UNIT_ICON},
/* 84 */		{/*"Linnias Baccaere",*/	REALM::HIDDEN_TEMPLE_OF_CUIRAECEN,			ANUIREAN,	PRIEST,	7,		NO_CLASS,	0,	MINOR,	REYNIR,		23,	0,	1071,	REG_PRSTM2_UNIT_ICON},
/* 85 */		{/*"Moerele Lannaman",*/	REALM::MHORIED,									ANUIREAN,	THIEF,	4,		NO_CLASS,	0,	MAJOR,	BRENNA,		29,	0,	1072,	REG_GILDM2_UNIT_ICON},
/* 86 */		{/*"Orthien Tane",*/			REALM::S_ANUIRE_SHIPPING_AND_IMPORTS,		ANUIREAN,	THIEF,	3,		NO_CLASS,	0,	TAINTED,	ANDUIRAS,	8,		0,	1073,	REG_GILDM1_UNIT_ICON},
/* 87 */		{/*"Regien",*/					REALM::MHORIED,									ANUIREAN,	WIZARD,	6,		NO_CLASS,	0,	MAJOR,	VORYNN,		30,	0,	1075,	REG_WIZDM1_UNIT_ICON},
/* 88 */		{/*"Rhobher Nichaleir",*/	REALM::AVANIL,										ANUIREAN,	PRIEST,	13,	NO_CLASS,	0,	GREAT,	ANDUIRAS,	53,	0,	1076,	REG_WIZDM1_UNIT_ICON},
/* 89 */		{/*"Ruarch Rockhammer",*/	REALM::RUARCH_ROCKHAMMER,						ANUIREAN,	PRIEST,	11,	NO_CLASS,	0,	MAJOR,	ANDUIRAS,	41,	0,	1077,	REG_DLORD3_UNIT_ICON},
/* 90 */		{/*"The Swamp Mage",*/		REALM::WIZARD_SECOND_SWAMP_MAGE,				ANUIREAN,	WIZARD,	10,	NO_CLASS,	0,	TAINTED,	VORYNN,		15,	0,	1078,	REG_WIZDF2_UNIT_ICON},
/* 91 */		{/*"Temias Coumain",*/		REALM::CELESTIAL_JEWEL_OF_SARIMIE,			ANUIREAN,	PRIEST,	8,		NO_CLASS,	0,	MINOR,	BRENNA,		18,	0,	1079,	REG_PRSTM1_UNIT_ICON},
/* 92 */		{/*"Torias Griene",*/		REALM::TALINIE,									ANUIREAN,	PRIEST,	7,		NO_CLASS,	0,	MINOR,	REYNIR,		15,	0,	1080,	REG_PRSTM2_UNIT_ICON},
/* 93 */		{/*"",*/ REALM::MIERES,		0,	0,	0,	0,	0,	0,	0,	0,	0, 1999, A_BOW_UNIT_ICON},

/* Generic Lt. NPC's */

/* 94 */		{/*"Alark Mohair",*/		REALM::DHOESONE,	ANUIREAN,	FIGHTER,	9,		NO_CLASS,	0,	TAINTED,	ANDUIRAS,		5,		0,		2000,	REG_LORDM3_UNIT_ICON},
/* 95 */		{/*"Haftbane",*/			REALM::BOERUINE,	ANUIREAN,	FIGHTER,	4,		NO_CLASS,	0,	TAINTED,	ANDUIRAS,		5,		0,		2001,	REG_LORDM2_UNIT_ICON},
/* 96 */		{/*"Darian Gondule",*/	REALM::AVANIL,	ANUIREAN,	FIGHTER,	9,		NO_CLASS,	0,	TAINTED,	ANDUIRAS,		5,		0,		2002,	REG_LORDM4_UNIT_ICON},
/* 97 */		{/*"Edwin Kermain",*/	REALM::TAEGHAS,	ANUIREAN,	FIGHTER,	5,		NO_CLASS,	0,	TAINTED,	ANDUIRAS,		5,		0,		2003,	REG_LORDM1_UNIT_ICON},
/* 98 */		{/*"Regial Strom",*/		REALM::OSOERDE,	ANUIREAN,	FIGHTER,	6,		NO_CLASS,	0,	TAINTED,	ANDUIRAS,		5,		0,		2004,	REG_LORDM2_UNIT_ICON},
/* 99 */		{/*"Durene Parien",*/	REALM::AERENWE,	ANUIREAN,	FIGHTER,	7,		NO_CLASS,	0,	TAINTED,	ANDUIRAS,		5,		0,		2005,	REG_LORDM7_UNIT_ICON},
/* 100 */		{/*"Faelan Kasarel",*/	REALM::TUARHIEVEL,	ANUIREAN,	FIGHTER,	9,		NO_CLASS,	0,	TAINTED,	ANDUIRAS,		5,		0,		2006,	REG_LORDM2_UNIT_ICON},
/* 101 */		{/*"Noelon Caesen",*/	REALM::ENDIER,	ANUIREAN,	FIGHTER,	4,		NO_CLASS,	0,	TAINTED,	ANDUIRAS,		5,		0,		2007,	REG_LORDM3_UNIT_ICON},
/*102 */		{/*"Gwenevier Lial",*/	REALM::ENDIER,	ANUIREAN,	FIGHTER,	5,		NO_CLASS,	0,	TAINTED,	ANDUIRAS,		5,		0,		2008,	REG_LORDF1_UNIT_ICON},
/*103 */		{/*"Bristim",*/			REALM::COERANYS,	ANUIREAN,	FIGHTER,	4,		NO_CLASS,	0,	TAINTED,	ANDUIRAS,		5,		0,		2009,	REG_LORDF3_UNIT_ICON},
/*104 */		{/*"Barruivan",*/			REALM::TALINIE,	ANUIREAN,	FIGHTER,	4,		NO_CLASS,	0,	TAINTED,	ANDUIRAS,		5,		0,		2010,	REG_LORDF3_UNIT_ICON},
/*105 */		{/*"Brienae",*/			REALM::BROSENGAE,	ANUIREAN,	PRIEST,	6,		NO_CLASS,	0,	TAINTED,	REYNIR,			5,		0,		2011,	REG_PRSTM2_UNIT_ICON},
/*106 */		{/*"Daegarinn",*/			REALM::MEDOERE,	ANUIREAN,	PRIEST,	4,		NO_CLASS,	0,	TAINTED,	REYNIR,			5,		0,		2012,	REG_PRSTM1_UNIT_ICON},
/*107 */		{/*"Britker Fraela",*/	REALM::ILIEN,	ANUIREAN,	PRIEST,	7,		NO_CLASS,	0,	TAINTED,	REYNIR,			5,		0,		2013,	REG_PRSTM2_UNIT_ICON},
/*108 */		{/*"Kalstra",*/			REALM::ELINIE,	ANUIREAN,	PRIEST,	8,		NO_CLASS,	0,	TAINTED,	REYNIR,			5,		0,		2014,	REG_PRSTF1_UNIT_ICON},
/*109 */		{/*"Darnae Eveard",*/	REALM::MHORIED,	ANUIREAN,	PRIEST,	4,		NO_CLASS,	0,	TAINTED,	REYNIR,			5,		0,		2015,	REG_WIZDF2_UNIT_ICON},
/*110 */		{/*"Shaltorn",*/			REALM::SIELWODE,	ANUIREAN,	RANGER,	5,		NO_CLASS,	0,	TAINTED,	REYNIR,			5,		0,		2016,	REG_WIZDM1_UNIT_ICON},
/*111 */		{/*"Cheassal Brodic",*/	REALM::DIEMED,	ANUIREAN,	WIZARD,	6,		NO_CLASS,	0,	TAINTED,	VORYNN,			5,		0,		2017,	REG_WIZDM1_UNIT_ICON},
/*112 */		{/*"Raile Valdhuine",*/	REALM::ROESONE,	ANUIREAN,	WIZARD,	4,		NO_CLASS,	0,	TAINTED,	VORYNN,			5,		0,		2018,	REG_WIZDM2_UNIT_ICON},
/*113 */		{/*"Rodel Garwen",*/		REALM::ENDIER,	ANUIREAN,	WIZARD,	7,		NO_CLASS,	0,	TAINTED,	VORYNN,			5,		0,		2019,	REG_WIZDM2_UNIT_ICON},
/*114 */		{/*"Moerde Ranael",*/	REALM::TUORNEN,	ANUIREAN,	WIZARD,	8,		NO_CLASS,	0,	TAINTED,	VORYNN,			5,		0,		2020,	REG_WIZDM3_UNIT_ICON},
/*115 */		{/*"Shaene Braesa",*/	REALM::CARIELE,	ANUIREAN,	WIZARD,	5,		NO_CLASS,	0,	TAINTED,	VORYNN,			5,		0,		2021,	REG_WIZDF2_UNIT_ICON},
/*116 */		{/*"Berwinde Dargan",*/	REALM::AERENWE,	ANUIREAN,	WIZARD,	5,		NO_CLASS,	0,	TAINTED,	VORYNN,			5,		0,		2022,	REG_WIZDF1_UNIT_ICON},
/*117 */		{/*"Jahstael Donat",*/	REALM::ROESONE,	ANUIREAN,	WIZARD,	4,		NO_CLASS,	0,	TAINTED,	VORYNN,			5,		0,		2023,	REG_PRSTF1_UNIT_ICON},

/* Remember to update CHARACTER_COUNT in gamemap.hxx, to be one higher than the last number listed above */
};


DEFINE_VECTOR_CLASS(REGENT,regents);

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
extern LONG JoinUnits_sub (LONG iUnit1, LONG iUnit2, BOOL fSend);
extern BOOL CheckUnitStack (LONG iUnit, BOOL fSend);

/* ========================================================================
   Function    - TransferRegentIconsToUnitsArray
   Description - Transfer the regent icons to the units array (duhhh)
   Returns     - void
   ======================================================================== */
void TransferRegentIconsToUnitsArray(void)
{
}


/* ========================================================================
   Function    - GetNextUnitInstance
   Description - Helper function for LinkUnits and ProvinceIsLinked:  This
                 function goes scans the units array, given a starting index,
                 and looks for the next unjoined unit in a given province.
   Returns     - The next units index, MAX_UNITS if no unit was found
   ======================================================================== */
static LONG GetNextUnitInstance(PROVINCE Prov,LONG startIdx)
{
	return 0;
}
	


/* ========================================================================
   Function    - ProvinceIsLinked
   Description - determines whether there are two armies in the units table
                 that are in the same province, yet aren't linked.
   Returns     - TRUE is the province is all linked correctly.
   ======================================================================== */
static BOOL ProvinceIsLinked(PROVINCE Prov)
{
	return FALSE;
}
/* ========================================================================
   Function    - LinkUnits
   Description - Goes through provinces and links their units
               - Used only at start of game to init stacks
   Returns     - void
   ======================================================================== */
void LinkUnits (BOOL)
{
}

/* ========================================================================
   Function    - RegentUnitOfRealm
   Description - return the regent of the given realm
   Returns     - unit index number
   ======================================================================== */
SHORT RegentUnitOfRealm(SHORT RealmId)
{
	return 0;
}

/* ========================================================================
   Function    - IsRegent
   Description - Checks whether a unit is a regent.
   Returns     - TRUE if unit is a regent, FALSE if not.
   ======================================================================== */
BOOL IsRegent (LONG UnitIndex)
{
	return FALSE;
}


/* ========================================================================
   Function    - IsLieutenant
   Description - Checks whether a unit is a regent.
   Returns     - TRUE if unit is a regent, FALSE if not.
   ======================================================================== */
BOOL IsLieutenant (LONG UnitIndex)
{
	return FALSE;
}


/* ========================================================================
   Function    - IsRegentOrLieutenant
   Description - Checks whether a unit is a regent or a lieutenant.
   Returns     - TRUE if unit is a regent or lieutenant, FALSE if not.
   ======================================================================== */
BOOL IsRegentOrLieutenant (LONG UnitIndex)
{
	return FALSE;
}


/* ========================================================================
   Function    - mfGetname
   Description - Get the name of the regent
   Returns     - pointer to the name (NULL if not found)
	Notes:
	[d4-09-97, 4-23-97 JPC] Modified to use mfGetName in player stats.
	Moved from the HXX file to here to avoid recompiling everything when
	we fiddle with it.  Also: it is now more complicated than a typical
	inline function, and it requires #including PLAYSTAT.HXX.
	There's a comment in LoadStats that says anything that knows it's a regent
	should not be calling LoadStats, but I haven't found how else to get hold
	of the stats.
   ======================================================================== */
char const * const REGENT::mfGetname() const
{
	return NULL;
}

/* ======================================================================== */
