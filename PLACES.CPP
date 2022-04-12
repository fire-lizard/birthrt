#include <stdio.h>
#include <string.h>

#include "PLACES.HXX"

#include "SYSTEM.H"
#include "REALM.HXX"
#include "MAPICON.HXX"

/* PLACES LIST: PROVINCES AND WHO CONTROLS WHAT IN IT */

DEFINE_VECTOR_DATA_S(PLACE_TYPE, places, MAX_PLACES) = {       

/*  0 */   {    0,     0,  REALM::MONSTERS,                                1,  NO_MAP_ICON,  0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},

/*---------- ROESONE ----------*/
/*---------- Abbatuor  (Roesone) ----------*/
/*  1 */   { 1044,  1785,  REALM::ROESONE,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/*  2 */   { 1064,  1782,  REALM::IMPREGNABLE_HEART_OF_HAELYN,             2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/*  3 */   { 1017,  1785,  REALM::S_ANUIRE_SHIPPING_AND_IMPORTS,           3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/*  4 */   { 1025,  1851,  REALM::PORT_OF_CALL_EXCHANGE,                   0,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/*  5 */   {  994,  1809,  REALM::HIGH_MAGE_AELIES,                        2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/*  6 */   { 1060,  1818,  REALM::ILIEN,                                   2,  SOURCE1_ICON,90,0,0,0,0,1,0, 0,  6, NULL     , fERROR},
/*  7 */   { 1072,  1840,  REALM::ROESONE,                                 0,  RUINS_ICON,   0,0,0,0,0,0,0,60,  7,"winte_xx", fERROR},
/*  8 */   { 1001,  1847+4,REALM::ROESONE,                                 1,  SEAPORT_ICON, 0,0,0,0,0,1,0,49,  0, NULL     , fERROR},
/*---------- Bellam  (Roesone) ----------*/                                                  
/*  9 */   { 1135,  1552,  REALM::ROESONE,                                 2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 10 */   { 0000,  0000,  REALM::IMPREGNABLE_HEART_OF_HAELYN,             3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 11 */   { 0000,  0000,  REALM::S_ANUIRE_SHIPPING_AND_IMPORTS,           3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 12 */   { 0000,  0000,  REALM::HIGH_MAGE_AELIES,                        2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,   0, NULL     , fERROR},
/*---------- Caercas  (Roesone) ----------*/                                                 
/* 13 */   {  976,  1653,  REALM::ROESONE,                                 3,  CASTLE1_ICON, 0,0,0,0,0,1,0, 1,  1, NULL     , fERROR},
/* 14 */   {  897,  1603,  REALM::ROESONE,                                 2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 15 */   { 0000,  0000,  REALM::IMPREGNABLE_HEART_OF_HAELYN,             3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 16 */   { 0000,  0000,  REALM::RUORNILS_CELESTIAL_SPELL,                1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 17 */   {  928,  1661,  REALM::PORT_OF_CALL_EXCHANGE,                   2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 18 */   { 0000,  0000,  REALM::S_ANUIRE_SHIPPING_AND_IMPORTS,           0,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  6, NULL     , fERROR},
/* 19 */   {  961,  1677,  REALM::ILIEN,                                   1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Duerlin  (Roesone) ----------*/                                                 
/* 20 */   { 1012,  1702,  REALM::ROESONE,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 21 */   { 0000,  0000,  REALM::IMPREGNABLE_HEART_OF_HAELYN,             2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 22 */   { 0000,  0000,  REALM::PORT_OF_CALL_EXCHANGE,                   3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 23 */   { 0000,  0000,  REALM::ILIEN,                                   2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Edlin  (Roesone) ----------*/                                                   
/* 24 */   { 1021,  1643,  REALM::ROESONE,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 25 */   { 1062,  1684,  REALM::IMPREGNABLE_HEART_OF_HAELYN,             2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 26 */   { 1022,  1671,  REALM::PORT_OF_CALL_EXCHANGE,                   3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 27 */   { 1106,  1647,  REALM::HIGH_MAGE_AELIES,                        2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Fairfield  (Roesone) ----------*/                                               
/* 28 */   { 1021,  1517,  REALM::ROESONE,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 29 */   { 1038,  1541,  REALM::IMPREGNABLE_HEART_OF_HAELYN,             2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 30 */   { 1022,  1602,  REALM::S_ANUIRE_SHIPPING_AND_IMPORTS,           0,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 31 */   { 1028,  1522,  REALM::HIGH_MAGE_AELIES,                        2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Ghoried  (Roesone) ----------*/                                                 
/* 32 */   {  990,  1530,  REALM::ROESONE,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 33 */   {  909,  1548,  REALM::IMPREGNABLE_HEART_OF_HAELYN,             1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 34 */   {  999,  1580,  REALM::HIGH_MAGE_AELIES,                        2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- AERENWE ----------*/                                                            
/*---------- Banien's Deep  (Aerenwe) ----------*/                                           
/* 35 */   { 0000,  0000,  REALM::AERENWE,                                 0,  RUINS_ICON,   0,0,0,0,0,1,0,51,  1, NULL/*"banie_xx"*/, fERROR},
/* 36 */   { 0000,  0000,  REALM::AERENWE,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 37 */   { 0000,  0000,  REALM::HIGH_MAGE_AELIES,                        3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 38 */   { 0000,  0000,  REALM::ILIEN,                                   3,  SOURCE1_ICON,52,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Calrie  (Aerenwe) ----------*/                                                  
/* 39 */   { 1266,  1666,  REALM::AERENWE,                                 6,  CASTLE1_ICON, 0,0,0,0,0,1,0, 2,  1,"calri_kw", fERROR},
/* 40 */   { 0000,  0000,  REALM::AERENWE,                                 6,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 41 */   { 0000,  0000,  REALM::HIGH_MAGE_AELIES,                        0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 42 */   { 1266,  1591-11,REALM::AERENWE,                                0,  PORT_ICON,    0,0,0,0,0,1,0,32,  0, NULL     , fERROR},
/*---------- Dhoenel  (Aerenwe) ----------*/                                                 
/* 43 */   { 0000,  0000,  REALM::AERENWE,                                 5,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 44 */   { 0000,  0000,  REALM::HIGH_MAGE_AELIES,                        0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Halried  (Aerenwe) ----------*/                                                 
/* 45 */   { 1223,  1691,  REALM::AERENWE,                                 5,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 46 */   { 1119,  1737,  REALM::HIGH_MAGE_AELIES,                        0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Northvale  (Aerenwe) ----------*/                                               
/* 47 */   { 0000,  0000,  REALM::AERENWE,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 48 */   { 0000,  0000,  REALM::HIGH_MAGE_AELIES,                        5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 49 */   { 0000,  0000,  REALM::ILIEN,                                   1,  SOURCE1_ICON,38,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Shadowgreen  (Aerenwe) ----------*/                                             
/* 50 */   { 0000,  0000,  REALM::AERENWE,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 51 */   { 1291,  1834,  REALM::HIGH_MAGE_AELIES,                        5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 52 */   { 0000,  0000,  REALM::ILIEN,                                   1,  SOURCE1_ICON,55,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Westmarch  (Aerenwe) ----------*/                                               
/* 53 */   { 0000,  0000,  REALM::AERENWE,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 54 */   { 0000,  0000,  REALM::HIGH_MAGE_AELIES,                        3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 55 */   { 0000,  0000,  REALM::ILIEN,                                   3,  SOURCE1_ICON, 6,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- DIEMED ----------*/                                                             
/*---------- Aerele  (Diemed) ----------*/                                                   
/* 56 */   { 0000,  0000,  REALM::DIEMED,                                  4,  CASTLE1_ICON, 0,0,0,0,0,1,0, 3,  1,"aerel_mw", fERROR},
/* 57 */   {  672,  1754,  REALM::DIEMED,                                  3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 58 */   { 0000,  0000,  REALM::ORTHODOX_IMPERIAL_TEMPLE_OF_HAELYN,      4,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 59 */   { 0000,  0000,  REALM::AVANIL,                                  0,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 60 */   { 0000,  0000,  REALM::PORT_OF_CALL_EXCHANGE,                   2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 61 */   {  688,  1759,  REALM::WIZARD_HERMEDHIE,                        1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  6, NULL     , fERROR},
/* 62 */   { 0000,  0000,  REALM::S_ANUIRE_SHIPPING_AND_IMPORTS,           2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  7, NULL     , fERROR},
/* 63 */   {  652,  1748,  REALM::DIEMED,                                  0,  SEAPORT_ICON, 0,0,0,0,0,1,0,33,  0, NULL     , fERROR},
/*---------- Bliene  (Diemed) ----------*/                                                   
/* 64 */   { 0000,  0000,  REALM::DIEMED,                                  2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 65 */   { 0000,  0000,  REALM::ORTHODOX_IMPERIAL_TEMPLE_OF_HAELYN,      3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 66 */   { 0000,  0000,  REALM::PORT_OF_CALL_EXCHANGE,                   3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 67 */   { 0000,  0000,  REALM::WIZARD_HERMEDHIE,                        4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Ciliene  (Diemed) ----------*/                                                  
/* 68 */   { 0000,  0000,  REALM::DIEMED,                                  4,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 69 */   {  590,  1607,  REALM::ORTHODOX_IMPERIAL_TEMPLE_OF_HAELYN,      6,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 70 */   { 0000,  0000,  REALM::AVANIL,                                  2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 71 */   { 0000,  0000,  REALM::PORT_OF_CALL_EXCHANGE,                   3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 72 */   {  499,  1615,  REALM::ENDIER_HEARTLANDS_OUTFITTERS,            3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Duene  (Diemed) ----------*/                                                    
/* 73 */   {  633,  1618,  REALM::DIEMED,                                  3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 74 */   {  692,  1700,  REALM::ORTHODOX_IMPERIAL_TEMPLE_OF_HAELYN,      3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 75 */   {  662,  1631,  REALM::S_ANUIRE_SHIPPING_AND_IMPORTS,           3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 76 */   { 0000,  0000,  REALM::WIZARD_HERMEDHIE,                        2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Moere  (Diemed) ----------*/                                                    
/* 77 */   { 0000,  0000,  REALM::DIEMED,                                  3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 78 */   { 0000,  0000,  REALM::AVANIL,                                  1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 79 */   {  679,  1519,  REALM::ORTHODOX_IMPERIAL_TEMPLE_OF_HAELYN,      5,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 80 */   { 0000,  0000,  REALM::ENDIER_HEARTLANDS_OUTFITTERS,            4,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 81 */   { 0000,  0000,  REALM::S_ANUIRE_SHIPPING_AND_IMPORTS,           1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Tier  (Diemed) ----------*/                                                     
/* 82 */   { 0000,  0000,  REALM::DIEMED,                                  1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 83 */   { 0000,  0000,  REALM::ORTHODOX_IMPERIAL_TEMPLE_OF_HAELYN,      2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 84 */   { 0000,  0000,  REALM::ENDIER_HEARTLANDS_OUTFITTERS,            2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 85 */   { 0000,  0000,  REALM::WIZARD_HERMEDHIE,                        1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- ILIEN ----------*/                                                              
/*----------   Illien  (Illien) ----------*/                                                 
/* 86 */   { 0000,  0000,  REALM::ILIEN,                                   7,  CASTLE1_ICON, 0,0,0,0,0,1,0, 4,  1,"ilien_mw", fERROR},
/* 87 */   { 0000,  0000,  REALM::ILIEN,                                   7,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 88 */   {  901,  1699,  REALM::IMPREGNABLE_HEART_OF_HAELYN,             4,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 89 */   {  944,  1773,  REALM::PORT_OF_CALL_EXCHANGE,                   7,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 90 */   {  935,  1813,  REALM::ILIEN,                                   0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 91 */   {919-24, 1779,  REALM::ILIEN,                                   0,  SEAPORT_ICON, 0,0,0,0,0,1,0,34,  0, NULL     , fERROR},
/*---------- MEDOERE ----------*/                                                            
/*---------- Alamier  (Medoere) ----------*/                                                 
/* 92 */   { 0000,  0000,  REALM::MEDOERE,                               		3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 93 */   { 0000,  0000,  REALM::RUORNILS_CELESTIAL_SPELL,                4,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 94 */   { 0000,  0000,  REALM::PORT_OF_CALL_EXCHANGE,                   2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 95 */   { 0000,  0000,  REALM::IMPREGNABLE_HEART_OF_HAELYN,             0,  TEMPLE1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 96 */   { 0000,  0000,  REALM::ILIEN,                                   1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 97 */   { 0000,  0000,  REALM::S_ANUIRE_SHIPPING_AND_IMPORTS,           2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Braeme  (Medoere) ----------*/                                                  
/* 112 */   {  831,  1660,  REALM::MEDOERE,                                3,  CASTLE1_ICON, 0,0,0,0,0,1,0, 5,  1,"braem_mw", fERROR},
/* 113 */   {  855,  1711,  REALM::MEDOERE,                                1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 114 */   {  826,  1697,  REALM::RUORNILS_CELESTIAL_SPELL,               3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 115 */   {  846,  1733,  REALM::S_ANUIRE_SHIPPING_AND_IMPORTS,          3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 116 */   { 0000,  0000,  REALM::WIZARD_HERMEDHIE,                       2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 117 */   {  824,  1732+4,REALM::MEDOERE,                                0,  SEAPORT_ICON, 0,0,0,0,0,1,0,35,  0, NULL     , fERROR},
/*---------- Caerwil  (Medoere) ----------*/                                                 
/* 118 */   { 0000,  0000,  REALM::ENDIER_HEARTLANDS_OUTFITTERS,           2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 119 */   { 0000,  0000,  REALM::RUORNILS_CELESTIAL_SPELL,               2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 120 */   { 0000,  0000,  REALM::ENDIER_HEARTLANDS_OUTFITTERS,           2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 121 */   { 0000,  0000,  REALM::WIZARD_HERMEDHIE,                       3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- SPIDERFELL, THE ----------*/                                                    
/*---------- Spiderfell  (Spiderfell) ----------*/                                           
/* 122 */   {  788,  1500,  REALM::SPIDERFELL,                             7,  CASTLE2_ICON,0,0,0,0,0,1,0, 7,  1,"spidfell", fERROR},
/* 123 */   { 0000,  0000,  REALM::SPIDERFELL,                             0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 124 */   { 0000,  0000,  REALM::ENDIER,                                 7,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- BOERUINE ----------*/                                                           
/*---------- Bacaele  (Boeruine) ----------*/                                                
/* 125 */   {  216,  1015,  REALM::BOERUINE,                               3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 126 */   {  136,  1007,  REALM::HIDDEN_TEMPLE_OF_CUIRAECEN,             3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 127 */   {  148,  1070,  REALM::BOERUINE,                               3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 128 */   {  200,  1054,  REALM::BOERUINE,                               2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Calant  (Boeruine) ----------*/                                                 
/* 129 */   { 0000,  0000,  REALM::BOERUINE,                               3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 130 */   { 0000,  0000,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 131 */   { 0000,  0000,  REALM::HIDDEN_TEMPLE_OF_CUIRAECEN,             0,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 132 */   { 0000,  0000,  REALM::BOERUINE,                               3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 133 */   { 0000,  0000,  REALM::BOERUINE,                               4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Dhalaese  (Boeruine) ----------*/                                               
/* 134 */   { 0000,  0000,  REALM::BOERUINE,                               3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 135 */   { 0000,  0000,  REALM::HIDDEN_TEMPLE_OF_CUIRAECEN,             3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 136 */   { 0000,  0000,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            0,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 137 */   { 0000,  0000,  REALM::UPPER_ANUIRE_TRADERS,                   3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 138 */   { 0000,  0000,  REALM::BOERUINE,                               0,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 139 */   { 0000,  0000,  REALM::BOERUINE,                               4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Fhoruile  (Boeruine) ----------*/                                               
/* 140 */   {  292,   758,  REALM::BOERUINE,                               3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/*     */   {  354,   802,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/*     */   {  292,   852,  REALM::BOERUINE,                               3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 141 */   {  318,   842,  REALM::BOERUINE,                               4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Nietier  (Boeruine) ----------*/                                                
/* 142 */   { 0000,  0000,  REALM::BOERUINE,                               3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 143 */   { 0000,  0000,  REALM::HIDDEN_TEMPLE_OF_CUIRAECEN,             3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 144 */   { 0000,  0000,  REALM::UPPER_ANUIRE_TRADERS,                   3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 145 */   { 0000,  0000,  REALM::BOERUINE,                               4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Redoubt  (Boeruine) ----------*/                                                
/*     */   { 0000,  0000,  REALM::BOERUINE,                               1,  CASTLE1_ICON, 0,0,0,0,0,1,0,61,  1,"bord_kg",  fERROR},
/* 146 */   { 0000,  0000,  REALM::BOERUINE,                               3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 147 */   { 0000,  0000,  REALM::HIDDEN_TEMPLE_OF_CUIRAECEN,             3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 148 */   { 0000,  0000,  REALM::BOERUINE,                               3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 149 */   { 0000,  0000,  REALM::UPPER_ANUIRE_TRADERS,                   0,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 150 */   { 0000,  0000,  REALM::BOERUINE,                               4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Rivien  (Boeruine) ----------*/                                                 
/*     */   { 0000,  0000,  REALM::BOERUINE,                               4,  CASTLE1_ICON, 0,0,0,0,0,1,0,59,  1,"black_pw", fERROR},
/* 151 */   { 0000,  0000,  REALM::BOERUINE,                               6,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 152 */   { 0000,  0000,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            6,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 153 */   {  369,   817,  REALM::BOERUINE,                               5,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 154 */   { 0000,  0000,  REALM::UPPER_ANUIRE_TRADERS,                   1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 155 */   { 0000,  0000,  REALM::BOERUINE,                               1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Seasedge  (Boeruine) ----------*/                                               
/* 156 */   {  223,   973,  REALM::BOERUINE,                               6,  CASTLE1_ICON, 0,0,0,0,0,1,0, 8,  1, NULL     , fERROR},
/* 157 */   {  180,   912,  REALM::BOERUINE,                               6,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 158 */   { 0000,  0000,  REALM::HIDDEN_TEMPLE_OF_CUIRAECEN,             3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 159 */   { 0000,  0000,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 160 */   {  169,   953,  REALM::BOERUINE,                               5,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 161 */   {  181,   897,  REALM::BOERUINE,                               0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Tariene  (Boeruine) ----------*/                                                
/* 162 */   {  257,   875,  REALM::BOERUINE,                               6,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 163 */   { 0000,  0000,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            5,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 164 */   { 0000,  0000,  REALM::HIDDEN_TEMPLE_OF_CUIRAECEN,             1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 165 */   { 0000,  0000,  REALM::BOERUINE,                               4,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 166 */   { 0000,  0000,  REALM::BOERUINE,                               0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 167 */   {  162,   866,  REALM::BOERUINE,                               0,  SEAPORT_ICON, 0,0,0,0,0,1,0,36,  0, NULL     , fERROR},
/*---------- TALINIE ----------*/                                                            
/*---------- Freestead  (Talinie) ----------*/                                               
/* 169 */   {  462,   606,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 170 */   {  504,   647,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 171 */   {  487,   594,  REALM::TALINIE,                                5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Greensward  (Talinie) ----------*/                                              
/* 173 */   { 0000,  0000,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 174 */   {  290,   624,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 175 */   { 0000,  0000,  REALM::TALINIE,                                5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Ice Haven  (Talinie) ----------*/                                               
/* 177 */   {  353,   611,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            3,  CASTLE1_ICON, 0,0,0,0,0,1,0, 9,  1,"nowel_kw", fERROR},
/* 178 */   {  376,   487,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 179 */   {  384,   553,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 180 */   {  384,   581,  REALM::TALINIE,                                4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 181 */   {  289,   576,  REALM:: N_IMPERIAL_TEMPLE_OF_HAELYN,           0,  PORT_ICON,    0,0,0,0,0,1,0,37,  0, NULL     , fERROR},
/*---------- Lindholme  (Talinie) ----------*/                                               
/* 184 */   { 0000,  0000,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 185 */   { 0000,  0000,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            4,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 186 */   { 0000,  0000,  REALM::BOERUINE,                               2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 187 */   { 0000,  0000,  REALM::TALINIE,                                2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Seaport  (Talinie) ----------*/                                                 
/* 190 */   { 0000,  0000,  REALM::BOERUINE,                               1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 191 */   { 0000,  0000,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            0,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 192 */   { 0000,  0000,  REALM::TALINIE,                                3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Serimset  (Talinie) ----------*/                                                
/* 194 */   { 0000,  0000,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 195 */   { 0000,  0000,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 196 */   { 0000,  0000,  REALM::TALINIE,                                5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Winter's Deep  (Talinie) ----------*/                                           
/* 198 */   { 0000,  0000,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 199 */   { 0000,  0000,  REALM::N_IMPERIAL_TEMPLE_OF_HAELYN,            2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 200 */   { 0000,  0000,  REALM::TALINIE,                                4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- BROSENGAE ----------*/                                                          
/*---------- Bindier  (Brosengae) ----------*/                                               
/* 202 */   {  343,  1628,  REALM::BROSEN_ROYAL_GUILD,                     4,  CASTLE1_ICON, 0,0,0,0,0,1,0,10,  1,"bindi_mw", fERROR},
/* 203 */   { 0000,  0000,  REALM::BROSEN_ROYAL_GUILD,                     4,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 204 */   { 0000,  0000,  REALM::BROSEN_ROYAL_GUILD,                     4,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 205 */   {308-14, 1626,  REALM::BROSEN_ROYAL_GUILD,                     0,  SEAPORT_ICON, 0,0,0,0,0,1,0,38,  0, NULL     , fERROR},
/*---------- Coere  (Brosengae) ----------*/                                                 
/* 207 */   {  321,  1516,  REALM::AVANIL,                                 3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 208 */   {  405,  1480,  REALM::BROSEN_ROYAL_GUILD,                     1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 209 */   {  346,  1500,  REALM::BROSENGAE,                              2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 210 */   {  383,  1504,  REALM::BROSEN_ROYAL_GUILD,                     4,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Marilen  (Brosengae) ----------*/                                               
/* 212 */   { 0000,  0000,  REALM::AVANIL,                                 3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 213 */   { 0000,  0000,  REALM::BROSEN_ROYAL_GUILD,                     1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 214 */   { 0000,  0000,  REALM::BROSENGAE,                              2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 215 */   { 0000,  0000,  REALM::BROSEN_ROYAL_GUILD,                     4,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- RHUOBHE ----------*/                                                            
/*---------- RHUOBHE  (RHUOBHE) ----------*/                                                 
/* 216 */   {421-25,1169-25,REALM::RHUOBHE,                                7,  CASTLE3_ICON, 0,0,0,0,0,1,0,11,  1,"rhoub_kw", fERROR},
/* 217 */   {  381,  1095,  REALM::RHUOBHE,                                2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 218 */   {  438,  1141,  REALM::RHUOBHE,                                9,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- TAEGHAS ----------*/                                                            
/*---------- Bayside  (Taeghas) ----------*/                                                 
/* 219 */   {  234,  1256,  REALM::AVANIL,                                 2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 220 */   {  191,  1225,  REALM::BOERUINE,                               0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 221 */   {  273,  1220,  REALM::TAEGHAS,                                2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Bhaine  (Taeghas) ----------*/                                                  
/* 224 */   {  139,  1224,  REALM::AVANIL,                                 3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 225 */   {  122,  1285,  REALM::TAEGHAS,                                3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 226 */   {  127,  1254,  REALM::AVANIL,                                 2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 227 */   {  208,  1299,  REALM::TAEGHAS,                                0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Brosien  (Taeghas) ----------*/                                                 
/* 230 */   { 0000,  0000,  REALM::AVANIL,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 231 */   { 0000,  0000,  REALM::AVANIL,                                 2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 232 */   { 0000,  0000,  REALM::TAEGHAS,                                6,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Islien  (Taeghas) ----------*/                                                  
/* 234 */   { 0000,  0000,  REALM::AVANIL,                                 2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 235 */   { 0000,  0000,  REALM::TAEGHAS,                                4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Portage  (Taeghas) ----------*/                                                 
/* 238 */   { 0000,  0000,  REALM::AVANIL,                                 2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 239 */   { 0000,  0000,  REALM::BOERUINE,                               0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 240 */   { 0000,  0000,  REALM::TAEGHAS,                                2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Seamist  (Taeghas) ----------*/                                                 
/* 243 */   {  306,  1232,  REALM::AVANIL,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 244 */   {  317,  1295,  REALM::AVANIL,                                 2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 245 */   {  274,  1287,  REALM::TAEGHAS,                                6,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Seasdeep  (Taeghas) ----------*/                                                
/* 246 */   {   72,  1315,  REALM::TAEGHAS,                                3,  CASTLE1_ICON, 0,0,0,0,0,1,0,12,  1,"storm_kw", fERROR},
/* 247 */   {  193,  1369,  REALM::AVANIL,                                 2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 248 */   {  114,  1380,  REALM::TAEGHAS,                                4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 249 */   {89-24,  1352,  REALM::TAEGHAS,                                0,  SEAPORT_ICON, 0,0,0,0,0,1,0,39,  0, NULL     , fERROR},
/*---------- Wilder's Gorge  (Taeghas) ----------*/                                          
/* 252 */   { 0000,  0000,  REALM::AVANIL,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 253 */   { 0000,  0000,  REALM::AVANIL,                                 1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 254 */   { 0000,  0000,  REALM::BOERUINE,                               0,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 255 */   { 0000,  0000,  REALM::TAEGHAS,                                6,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- AVANIL ----------*/                                                             
/*---------- Anuire  (Avanil) ----------*/                                                   
/* 256 */   { 0000,  0000,  REALM::AVANIL,                                 7,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 257 */   { 0000,  0000,  REALM::AVANIL,                                 4,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 258 */   { 0000,  0000,  REALM::CELESTIAL_JEWEL_OF_SARIMIE,             3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 259 */   { 0000,  0000,  REALM::AVANIL,                                 4,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 260 */   { 0000,  0000,  REALM::BROSEN_ROYAL_GUILD,                     2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 261 */   { 0000,  0000,  REALM::TAEGHAS,                                0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Avarien  (Avanil) ----------*/                                                  
/* 263 */   { 0000,  0000,  REALM::AVANIL,                                 3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 264 */   { 0000,  0000,  REALM::AVANIL,                                 3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 265 */   { 0000,  0000,  REALM::AVANIL,                                 3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 266 */   { 0000,  0000,  REALM::TAEGHAS,                                3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Bhrein  (Avanil) ----------*/                                                   
/* 267 */   { 0000,  0000,  REALM::AVANIL,                                 4,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 268 */   { 0000,  0000,  REALM::CELESTIAL_JEWEL_OF_SARIMIE,             2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 269 */   { 0000,  0000,  REALM::AVANIL,                                 2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 270 */   { 0000,  0000,  REALM::BROSEN_ROYAL_GUILD,                     2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 271 */   { 0000,  0000,  REALM::AVANIL,                                 2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 272 */   { 0000,  0000,  REALM::TAEGHAS,                                1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Caulnor  (Avanil) ----------*/                                                  
/* 273 */   { 0000,  0000,  REALM::AVANIL,                                 5,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 274 */   { 0000,  0000,  REALM::AVANIL,                                 3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 275 */   { 0000,  0000,  REALM::CELESTIAL_JEWEL_OF_SARIMIE,             2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 276 */   { 0000,  0000,  REALM::AVANIL,                                 4,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 277 */   { 0000,  0000,  REALM::TAEGHAS,                                0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Daulton  (Avanil) ----------*/                                                  
/* 279 */   { 0000,  0000,  REALM::AVANIL,                                 5,  CASTLE1_ICON, 0,0,0,0,0,1,0,13,  1, NULL     , fERROR},
/* 280 */   { 0000,  0000,  REALM::AVANIL,                                 5,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 281 */   { 0000,  0000,  REALM::CELESTIAL_JEWEL_OF_SARIMIE,             3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 282 */   { 0000,  0000,  REALM::AVANIL,                                 2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 283 */   { 0000,  0000,  REALM::AVANIL,                                 3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 284 */   { 0000,  0000,  REALM::TAEGHAS,                                2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Duriene  (Avanil) ----------*/                                                  
/* 286 */   { 0000,  0000,  REALM::AVANIL,                                 4,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 287 */   { 0000,  0000,  REALM::CELESTIAL_JEWEL_OF_SARIMIE,             3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 288 */   { 0000,  0000,  REALM::AVANIL,                                 1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 289 */   { 0000,  0000,  REALM::AVANIL,                                 3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 290 */   { 0000,  0000,  REALM::BROSEN_ROYAL_GUILD,                     1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 291 */   { 0000,  0000,  REALM::TAEGHAS,                                1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Nentril  (Avanil) ----------*/                                                  
/* 292 */   { 0000,  0000,  REALM::AVANIL,                                 3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 293 */   { 0000,  0000,  REALM::AVANIL,                                 3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 294 */   { 0000,  0000,  REALM::AVANIL,                                 3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 295 */   { 0000,  0000,  REALM::TAEGHAS,                                4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Taliern  (Avanil) ----------*/                                                  
/* 296 */   { 0000,  0000,  REALM::AVANIL,                                 3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 297 */   { 0000,  0000,  REALM::AVANIL,                                 3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 298 */   { 0000,  0000,  REALM::AVANIL,                                 3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 299 */   { 0000,  0000,  REALM::BROSEN_ROYAL_GUILD,                     0,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 300 */   { 0000,  0000,  REALM::TAEGHAS,                                4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Vanilen  (Avanil) ----------*/                                                  
/*     */   {  362,  1280,  REALM::AVANIL,                                 1,  WIZTOWER_ICON,0,0,0,0,0,1,0,53,  1, NULL/*"drago_xx"*/, fERROR},
/* 301 */   {  350,  1356,  REALM::AVANIL,                                 3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 302 */   {  350,  1336,  REALM::AVANIL,                                 3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 303 */   { 0000,  0000,  REALM::AVANIL,                                 3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 304 */   { 0000,  0000,  REALM::TAEGHAS,                                4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- GHOERE ----------*/                                                             
/*---------- Achiese  (Ghoere) ----------*/                                                  
/* 305 */   { 0000,  0000,  REALM::GHOERE,                                 3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 306 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 307 */   { 0000,  0000,  REALM::COERANYS,                               1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 308 */   { 0000,  0000,  REALM::GHOERE,                                 4,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 309 */   { 0000,  0000,  REALM::GHOERE,                                 1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Bhalaene  (Ghoere) ----------*/                                                 
/* 310 */   { 0000,  0000,  REALM::GHOERE,                                 5,  CASTLE1_ICON, 0,0,0,0,0,1,0,14,  1, NULL     , fERROR},
/* 311 */   { 0000,  0000,  REALM::GHOERE,                                 4,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 312 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            5,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 313 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 314 */   { 0000,  0000,  REALM::GHOERE,                                 4,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 315 */   { 0000,  0000,  REALM::MHORIED,                                2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  6, NULL     , fERROR},
/* 316 */   { 0000,  0000,  REALM::GHOERE,                                 0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Bheline  (Ghoere) ----------*/                                                  
/* 317 */   { 0000,  0000,  REALM::GHOERE,                                 4,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 318 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 319 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 320 */   { 0000,  0000,  REALM::GHOERE,                                 4,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 321 */   { 0000,  0000,  REALM::GHOERE,                                 1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Conallier  (Ghoere) ----------*/                                                
/* 322 */   { 0000,  0000,  REALM::GHOERE,                                 2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 323 */   { 0000,  0000,  REALM::MHORIED,                                0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 324 */   { 0000,  0000,  REALM::COERANYS,                               2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 325 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            0,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 326 */   { 0000,  0000,  REALM::MHORIED,                                2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 327 */   { 0000,  0000,  REALM::GHOERE,                                 3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Danaroene  (Ghoere) ----------*/                                                
/* 328 */   { 0000,  0000,  REALM::GHOERE,                                 3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 329 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 330 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 331 */   { 0000,  0000,  REALM::GHOERE,                                 4,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 332 */   { 0000,  0000,  REALM::GHOERE,                                 1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Ghiere  (Ghoere) ----------*/                                                   
/* 333 */   { 0000,  0000,  REALM::GHOERE,                                 3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 334 */   { 0000,  0000,  REALM::MHORIED,                                2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 335 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 336 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 337 */   { 0000,  0000,  REALM::GHOERE,                                 3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 338 */   { 0000,  0000,  REALM::ENDIER_HEARTLANDS_OUTFITTERS,           2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  6, NULL     , fERROR},
/* 339 */   { 0000,  0000,  REALM::GHOERE,                                 0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  7, NULL     , fERROR},
/* 340 */   {  776,  1268,  REALM::GHOERE,                                 0,  PORT_ICON,    0,0,0,0,0,1,0,40,  0, NULL     , fERROR},
/*---------- Rhumannen  (Ghoere) ----------*/                                                
/*     */   { 0000,  0000,  REALM::GHOERE,                                 0,  RUINS_ICON,   0,0,0,0,0,0,0,58,  1,"shadc_mw", fERROR},
/* 341 */   { 0000,  0000,  REALM::GHOERE,                                 2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 342 */   { 0000,  0000,  REALM::MHORIED,                                2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 343 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 344 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 345 */   { 0000,  0000,  REALM::GHOERE,                                 3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  6, NULL     , fERROR},
/* 346 */   { 0000,  0000,  REALM::ENDIER_HEARTLANDS_OUTFITTERS,           1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  7, NULL     , fERROR},
/* 347 */   { 0000,  0000,  REALM::GHOERE,                                 1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Thoralinar  (Ghoere) ----------*/                                               
/* 348 */   { 0000,  0000,  REALM::GHOERE,                                 3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 349 */   { 0000,  0000,  REALM::COERANYS,                               3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 350 */   { 0000,  0000,  REALM::GHOERE,                                 3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 351 */   { 0000,  0000,  REALM::GHOERE,                                 2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Tireste  (Ghoere) ----------*/                                                  
/* 352 */   { 0000,  0000,  REALM::GHOERE,                                 5,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 353 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 354 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 355 */   { 0000,  0000,  REALM::ENDIER_HEARTLANDS_OUTFITTERS,           4,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 356 */   { 0000,  0000,  REALM::GHOERE,                                 1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 357 */   { 0000,  0000,  REALM::GHOERE,                                 0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Tornilen  (Ghoere) ----------*/                                                 
/* 358 */   { 0000,  0000,  REALM::GHOERE,                                 2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 359 */   { 0000,  0000,  REALM::MHORIED,                                0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 360 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 361 */   { 0000,  0000,  REALM::GHOERE,                                 2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 362 */   { 0000,  0000,  REALM::MHORIED,                                0,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 363 */   { 0000,  0000,  REALM::GHOERE,                                 2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- MHORIED ----------*/                                                            
/*---------- Balteruine  (Mhoried) ----------*/                                              
/* 364 */   { 0000,  0000,  REALM::MHORIED,                                1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 365 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 366 */   { 0000,  0000,  REALM::MHORIED,                                2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 367 */   { 0000,  0000,  REALM::MHORIED,                                3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Bevaldruor  (Mhoried) ----------*/                                              
/* 368 */   { 0000,  0000,  REALM::MHORIED,                                4,  CASTLE1_ICON, 0,0,0,0,0,1,0,15,  1, NULL     , fERROR},
/* 369 */   { 0000,  0000,  REALM::MHORIED,                                4,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 370 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 371 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 372 */   { 0000,  0000,  REALM::MHORIED,                                5,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 373 */   { 0000,  0000,  REALM::UPPER_ANUIRE_TRADERS,                   1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  6, NULL     , fERROR},
///* 374 */   { 0000,  0000,  REALM::MHORIED,                                0,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  7, NULL     , fERROR},
/* 375 */   { 0000,  0000,  REALM::MHORIED,                                3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Byrnnor  (Mhoried) ----------*/                                                 
/* 376 */   { 0000,  0000,  REALM::MHORIED,                                3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 377 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 379 */   { 0000,  0000,  REALM::MHORIED,                                3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 380 */   { 0000,  0000,  REALM::UPPER_ANUIRE_TRADERS,                   1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
///* 381 */   { 0000,  0000,  REALM::MHORIED,                                0,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 382 */   { 0000,  0000,  REALM::MHORIED,                                1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Cwlldon  (Mhoried) ----------*/                                                 
/* 383 */   { 0000,  0000,  REALM::MHORIED,                                3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 384 */   { 0000,  0000,  REALM::GHOERE,                                 0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 385 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 386 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            0,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 387 */   { 0000,  0000,  REALM::GHOERE,                                 2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 388 */   { 0000,  0000,  REALM::MHORIED,                                0,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  6, NULL     , fERROR},
/* 389 */   { 0000,  0000,  REALM::GHOERE,                                 2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  7, NULL     , fERROR},
/* 390 */   { 0000,  0000,  REALM::MHORIED,                                0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Dhalsiel  (Mhoried) ----------*/                                                
/* 391 */   { 0000,  0000,  REALM::MHORIED,                                2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 393 */   { 0000,  0000,  REALM::MHORIED,                                2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 394 */   { 0000,  0000,  REALM::MHORIED,                                3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Maesilar  (Mhoried) ----------*/                                                
/* 395 */   { 0000,  0000,  REALM::GHOERE,                                 3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 396 */   { 0000,  0000,  REALM::MHORIED,                                0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 397 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 398 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          0,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 399 */   { 0000,  0000,  REALM::MHORIED,                                3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
///* 400 */   { 0000,  0000,  REALM::MHORIED,                                0,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  6, NULL     , fERROR},
/* 401 */   { 0000,  0000,  REALM::MHORIED,                                2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Marloer's Gap  (Mhoried) ----------*/                                           
/* 402 */   { 0000,  0000,  REALM::MHORIED,                                1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 403 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 405 */   { 0000,  0000,  REALM::MHORIED,                                2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 406 */   { 0000,  0000,  REALM::MHORIED,                                3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Tenarien  (Mhoried) ----------*/                                                
/* 407 */   { 0000,  0000,  REALM::MHORIED,                                3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 408 */   { 0000,  0000,  REALM::GHOERE,                                 0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 409 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 410 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          0,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 411 */   { 0000,  0000,  REALM::GHOERE,                                 3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 412 */   { 0000,  0000,  REALM::MHORIED,                                0,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  6, NULL     , fERROR},
/* 413 */   { 0000,  0000,  REALM::GHOERE,                                 2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  7, NULL     , fERROR},
/* 414 */   { 0000,  0000,  REALM::MHORIED,                                0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  8, NULL     , fERROR},
/* 415 */   {  908,  1155,  REALM::MHORIED,                                0,  PORT_ICON,    0,0,0,0,0,1,0,41,  0, NULL     , fERROR},
/*---------- Torien's Watch  (Mhoried) ----------*/                                          
/*     */   { 0000,  0000,  REALM::MHORIED,                                2,  CASTLE1_ICON, 0,0,0,0,0,1,0,56,  1,"moons_kw", fERROR},
/* 416 */   { 0000,  0000,  REALM::MHORIED,                                2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 418 */   { 0000,  0000,  REALM::UPPER_ANUIRE_TRADERS,                   3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 419 */   { 0000,  0000,  REALM::MHORIED,                                6,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Winoene  (Mhoried) ----------*/                                                 
/* 420 */   { 0000,  0000,  REALM::MHORIED,                                3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 421 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 422 */   { 0000,  0000,  REALM::UPPER_ANUIRE_TRADERS,                   3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 423 */   { 0000,  0000,  REALM::MHORIED,                                0,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 424 */   { 0000,  0000,  REALM::MHORIED,                                2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- TUORNRN ----------*/                                                            
/*---------- Alamsreft  (Tuornen) ----------*/                                               
/* 425 */   { 0000,  0000,  REALM::TUORNEN,                                1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 426 */   { 0000,  0000,  REALM::AVANIL,                                 0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 427 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 428 */   { 0000,  0000,  REALM::AVANIL,                                 0,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 429 */   { 0000,  0000,  REALM::CARIELE,                                3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 430 */   { 0000,  0000,  REALM::ENDIER,                                 2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Elevesnemiere  (Tuornen) ----------*/                                           
/* 431 */   { 0000,  0000,  REALM::TUORNEN,                                2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 432 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 433 */   { 0000,  0000,  REALM::AVANIL,                                 2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 434 */   { 0000,  0000,  REALM::ENDIER,                                 5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Ghonallison  (Tuornen) ----------*/                                             
/* 435 */   { 0000,  0000,  REALM::BOERUINE,                               2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 436 */   { 0000,  0000,  REALM::TUORNEN,                                0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 437 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 438 */   { 0000,  0000,  REALM::CARIELE,                                2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Haesrien  (Tuornen) ----------*/                                                
/* 439 */   {  587,  1160,  REALM::TUORNEN,                                5,  CASTLE1_ICON, 0,0,0,0,0,1,0,16,  1,"haes_kw",  fERROR},
/* 440 */   {  555,  1159,  REALM::TUORNEN,                                3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 441 */   { 0000,  0000,  REALM::BOERUINE,                               2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 442 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 443 */   { 0000,  0000,  REALM::AVANIL,                                 2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 444 */   { 0000,  0000,  REALM::BROSEN_ROYAL_GUILD,                     3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  6, NULL     , fERROR},
/* 445 */   { 0000,  0000,  REALM::TUORNEN,                                2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  7, NULL     , fERROR},
/* 446 */   { 0000,  0000,  REALM::CARIELE,                                0,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  8, NULL     , fERROR},
/* 447 */   { 0000,  0000,  REALM::ENDIER,                                 0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  9, NULL     , fERROR},
/* 448 */   {  607,  1084,  REALM::TUORNEN,                                0,  PORT_ICON,    0,0,0,0,0,1,0,42,  0, NULL     , fERROR},
/*---------- Monsedge  (Tuornen) ----------*/                                                
/* 449 */   { 0000,  0000,  REALM::TUORNEN,                                1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 450 */   { 0000,  0000,  REALM::BOERUINE,                               0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 451 */   { 0000,  0000,  REALM::AVANIL,                                 2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 452 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            0,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 453 */   { 0000,  0000,  REALM::CARIELE,                                3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 454 */   { 0000,  0000,  REALM::RHUOBHE,                                2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Nabhriene  (Tuornen) ----------*/                                               
/* 455 */   { 0000,  0000,  REALM::AVANIL,                                 2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 456 */   { 0000,  0000,  REALM::TUORNEN,                                0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 457 */   { 0000,  0000,  REALM::AVANIL,                                 3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 458 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            0,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 459 */   { 0000,  0000,  REALM::BROSEN_ROYAL_GUILD,                     3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 460 */   { 0000,  0000,  REALM::AVANIL,                                 0,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  6, NULL     , fERROR},
/* 461 */   { 0000,  0000,  REALM::ENDIER,                                 2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Pechalinn  (Tuornen) ----------*/                                               
/* 462 */   { 0000,  0000,  REALM::TUORNEN,                                2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 463 */   { 0000,  0000,  REALM::BOERUINE,                               0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 464 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 465 */   { 0000,  0000,  REALM::AVANIL,                                 2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 466 */   { 0000,  0000,  REALM::RHUOBHE,                                5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Tuor's Hold  (Tuornen) ----------*/                                             
/* 467 */   { 0000,  0000,  REALM::AVANIL,                                 2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 468 */   { 0000,  0000,  REALM::TUORNEN,                                0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 469 */   { 0000,  0000,  REALM::AVANIL,                                 3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 470 */   { 0000,  0000,  REALM::AVANIL,                                 3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 471 */   { 0000,  0000,  REALM::ENDIER,                                 2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- ALAMIE ----------*/                                                             
/*---------- Alaroine  (Alamie) ----------*/                                                 
/* 472 */   { 0000,  0000,  REALM::ALAMIE,                                 4,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 473 */   { 0000,  0000,  REALM::AVANIL,                                 2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 474 */   { 0000,  0000,  REALM::CELESTIAL_JEWEL_OF_SARIMIE,             3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 475 */   { 0000,  0000,  REALM::GHOERE,                                 2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 476 */   { 0000,  0000,  REALM::CARIELE,                                3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Deseirain  (Alamie) ----------*/                                                
/* 477 */   { 0000,  0000,  REALM::ALAMIE,                                 3,  CASTLE1_ICON, 0,0,0,0,0,1,0,17,  1,"lofto_mw", fERROR},
/* 478 */   { 0000,  0000,  REALM::ALAMIE,                                 2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 479 */   { 0000,  0000,  REALM::AVANIL,                                 3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 480 */   { 0000,  0000,  REALM::CARIELE,                                3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 481 */   { 0000,  0000,  REALM::ENDIER,                                 2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 482 */   {  648,  1166,  REALM::ALAMIE,                                 0,  PORT_ICON,    0,0,0,0,0,1,0,43,  0, NULL     , fERROR},
/*---------- Hildon  (Alamie) ----------*/                                                   
/* 483 */   { 0000,  0000,  REALM::ALAMIE,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 484 */   { 0000,  0000,  REALM::CELESTIAL_JEWEL_OF_SARIMIE,             2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 485 */   { 0000,  0000,  REALM::CARIELE,                                2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Laraeth  (Alamie) ----------*/                                                  
/* 486 */   { 0000,  0000,  REALM::ALAMIE,                                 2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 487 */   { 0000,  0000,  REALM::ENDIER_HEARTLANDS_OUTFITTERS,           0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 488 */   { 0000,  0000,  REALM::CELESTIAL_JEWEL_OF_SARIMIE,             3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 489 */   { 0000,  0000,  REALM::GHOERE,                                 3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 490 */   { 0000,  0000,  REALM::ENDIER,                                 2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Maesford  (Alamie) ----------*/                                                 
/* 491 */   {  684,  1355,  REALM::ALAMIE,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 492 */   {  735,  1291,  REALM::ENDIER_HEARTLANDS_OUTFITTERS,           0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 493 */   {  721,  1345,  REALM::AVANIL,                                 2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 494 */   {  688,  1285,  REALM::ENDIER_HEARTLANDS_OUTFITTERS,           2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 495 */   {  748,  1321,  REALM::ENDIER,                                 3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Nortmoor  (Alamie) ----------*/                                                 
/* 496 */   { 0000,  0000,  REALM::CELESTIAL_JEWEL_OF_SARIMIE,             1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 497 */   { 0000,  0000,  REALM::CARIELE,                                1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Sorelies  (Alamie) ----------*/                                                 
/* 498 */   { 0000,  0000,  REALM::ALAMIE,                                 4,  CASTLE1_ICON, 0,0,0,0,0,1,0,18,  1, NULL     , fERROR},
/* 499 */   { 0000,  0000,  REALM::AVANIL,                                 1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 500 */   { 0000,  0000,  REALM::CARIELE,                                1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Soutmoor  (Alamie) ----------*/                                                 
/* 501 */   { 0000,  0000,  REALM::CELESTIAL_JEWEL_OF_SARIMIE,             2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 502 */   { 0000,  0000,  REALM::GHOERE,                                 2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Traiward  (Alamie) ----------*/                                                 
/* 503 */   { 0000,  0000,  REALM::ALAMIE,                                 2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 504 */   { 0000,  0000,  REALM::ENDIER_HEARTLANDS_OUTFITTERS,           0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 505 */   { 0000,  0000,  REALM::AVANIL,                                 3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 506 */   { 0000,  0000,  REALM::ENDIER_HEARTLANDS_OUTFITTERS,           3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 507 */   { 0000,  0000,  REALM::ENDIER,                                 2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- IMPERIAL CITY OF ANUIRE, THE ----------*/                                       
/*---------- Imperial City of Anuire  (City of Anuire) ----------*/                          
/* 508 */   { 0000,  0000,  REALM::ANUIRE,                                 7,  CASTLE1_ICON, 0,0,0,0,0,1,0,19,  1, NULL     , fERROR},
/* 509 */   { 0000,  0000,  REALM::ANUIRE,                                 3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 510 */   { 0000,  0000,  REALM::AVANIL,                                 3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 511 */   { 0000,  0000,  REALM::BOERUINE,                               2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 512 */   { 0000,  0000,  REALM::DIEMED,                                 2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 513 */   { 0000,  0000,  REALM::AVANIL,                                 3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  6, NULL     , fERROR},
/* 514 */   { 0000,  0000,  REALM::COERANYS,                               2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  7, NULL     , fERROR},
/* 515 */   { 0000,  0000,  REALM::MILITANT_ORDER_OF_CUIRAECEN,            2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  8, NULL     , fERROR},
/* 516 */   { 0000,  0000,  REALM::CELESTIAL_JEWEL_OF_SARIMIE,             1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  9, NULL     , fERROR},
/* 517 */   { 0000,  0000,  REALM::ENDIER_HEARTLANDS_OUTFITTERS,           2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0, 10, NULL     , fERROR},
/* 518 */   { 0000,  0000,  REALM::AVANIL,                                 2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0, 11, NULL     , fERROR},
/* 519 */   { 0000,  0000,  REALM::MHORIED,                                1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0, 12, NULL     , fERROR},
/* 521 */   {  518,  1537+20,REALM::ANUIRE,                                0,  SEAPORT_ICON, 0,0,0,0,0,1,0,43,  0, NULL     , fERROR},
/*---------- ELINIE ----------*/                                                             
/*---------- Ansien  (Elinie) ----------*/                                                   
/* 522 */   { 0000,  0000,  REALM::ELINIE,                                 5,  CASTLE1_ICON, 0,0,0,0,0,1,0,20,  1,"ansie_tw", fERROR},
/* 523 */   { 0000,  0000,  REALM::ELINIE,                                 3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 524 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 525 */   { 1305,  1226,  REALM::COERANYS,                               3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 526 */   { 1273,  1179,  REALM::POINTS_EAST_TRADING_GUILD,              5,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Chalsedon  (Elinie) ----------*/                                                
/* 527 */   { 0000,  0000,  REALM::ELINIE,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 528 */   { 1257,  1272,  REALM::HAELYNS_AEGIS,                          3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 529 */   { 1244,  1204,  REALM::POINTS_EAST_TRADING_GUILD,              3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 530 */   { 1281,  1298,  REALM::GHOERE,                                 2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Hope's Demise  (Elinie) ----------*/                                            
/* 531 */   { 0000,  0000,  REALM::ELINIE,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 532 */   { 0000,  0000,  REALM::COERANYS,                               2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 533 */   { 0000,  0000,  REALM::POINTS_EAST_TRADING_GUILD,              1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 534 */   { 0000,  0000,  REALM::WIZARD_SECOND_SWAMP_MAGE,               5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Mholien  (Elinie) ----------*/                                                  
/* 535 */   { 0000,  0000,  REALM::ELINIE,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 536 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 537 */   { 0000,  0000,  REALM::POINTS_EAST_TRADING_GUILD,              2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 538 */   { 0000,  0000,  REALM::WIZARD_SECOND_SWAMP_MAGE,               3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 539 */   { 0000,  0000,  REALM::MHORIED,                                0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Osoeriene  (Elinie) ----------*/                                                
/* 540 */   { 0000,  0000,  REALM::ELINIE,                                 3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 541 */   { 0000,  0000,  REALM::COERANYS,                               3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 542 */   { 0000,  0000,  REALM::POINTS_EAST_TRADING_GUILD,              3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 543 */   { 0000,  0000,  REALM::GHOERE,                                 2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Sendouras  (Elinie) ----------*/                                                
/* 544 */   { 0000,  0000,  REALM::ELINIE,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 545 */   { 0000,  0000,  REALM::COERANYS,                               2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 546 */   { 0000,  0000,  REALM::POINTS_EAST_TRADING_GUILD,              3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 547 */   { 0000,  0000,  REALM::GHOERE,                                 2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Soileite  (Elinie) ----------*/                                                 
/* 548 */   { 0000,  0000,  REALM::ELINIE,                                 1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 549 */   { 0000,  0000,  REALM::COERANYS,                               2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 550 */   { 0000,  0000,  REALM::POINTS_EAST_TRADING_GUILD,              2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 551 */   { 0000,  0000,  REALM::WIZARD_SECOND_SWAMP_MAGE,               3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- ENDIER ----------*/                                                             
/*---------- Endier  (Endier) ----------*/                                                   
/* 552 */   { 0000,  0000,  REALM::ENDIER_HEARTLANDS_OUTFITTERS,           6,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 553 */   { 0000,  0000,  REALM::CELESTIAL_JEWEL_OF_SARIMIE,             3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 554 */   { 0000,  0000,  REALM::AVANIL,                                 3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 555 */   { 0000,  0000,  REALM::ENDIER_HEARTLANDS_OUTFITTERS,           6,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 556 */   { 0000,  0000,  REALM::ENDIER,                                 0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 557 */   { 0000,  0000,  REALM::ENDIER,                                 1,  WIZTOWER_ICON,0,0,0,0,0,1,0,21,  6,"falcomw",  fERROR},
/* 558 */   {  644,  1458-50,REALM::ENDIER_HEARTLANDS_OUTFITTERS,          0,  PORT_ICON,    0,0,0,0,0,1,0,44,  0, NULL     , fERROR},
/*---------- DHOESONE ----------*/                                                           
/*---------- Bjondrig  (Dhoesone) ----------*/                                               
/* 559 */   { 0000,  0000,  REALM::DHOESONE,                               0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 561 */   { 0000,  0000,  REALM::DHOESONE,                               4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Dharilein  (Dhoesone) ----------*/                                              
/* 564 */   { 0000,  0000,  REALM::DHOESONE,                               1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 565 */   { 0000,  0000,  REALM::N_IMPORTS_EXPORTS,                      0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 567 */   { 0000,  0000,  REALM::N_IMPORTS_EXPORTS,                      1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 568 */   { 0000,  0000,  REALM::DHOESONE,                               4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Giant's Fastness  (Dhoesone) ----------*/                                       
/* 569 */   { 0000,  0000,  REALM::UPPER_ANUIRE_TRADERS,                   1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 571 */   { 0000,  0000,  REALM::UPPER_ANUIRE_TRADERS,                   1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 572 */   { 0000,  0000,  REALM::DHOESONE,                               5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Hidaele  (Dhoesone) ----------*/                                                
/* 573 */   { 0000,  0000,  REALM::DHOESONE,                               1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 575 */   { 0000,  0000,  REALM::DHOESONE,                               3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Nolien  (Dhoesone) ----------*/                                                 
/* 578 */   { 0000,  0000,  REALM::DHOESONE,                               2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 579 */   { 0000,  0000,  REALM::N_REFORMED_CHURCH_OF_SARIMIE,           3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 581 */   { 0000,  0000,  REALM::DHOESONE,                               2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 582 */   {  653,   398,  REALM::DHOESONE,                               0,  SEAPORT_ICON, 0,0,0,0,0,1,0,45,  0, NULL     , fERROR},
/*---------- Riveside  (Dhoesone) ----------*/                                               
/* 585 */   { 0000,  0000,  REALM::DHOESONE,                               1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 587 */   { 0000,  0000,  REALM::DHOESONE,                               3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Romiene  (Dhoesone) ----------*/                                                
/* 590 */   { 0000,  0000,  REALM::DHOESONE,                               0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 592 */   { 0000,  0000,  REALM::DHOESONE,                               4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Ruidewash  (Dhoesone) ----------*/                                              
/* 595 */   { 0000,  0000,  REALM::DHOESONE,                               1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 596 */   { 0000,  0000,  REALM::N_IMPORTS_EXPORTS,                      0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 598 */   { 0000,  0000,  REALM::N_REFORMED_CHURCH_OF_SARIMIE,           1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 599 */   { 0000,  0000,  REALM::N_IMPORTS_EXPORTS,                      2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 600 */   { 0000,  0000,  REALM::DHOESONE,                               5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* xxx */   { 0000,  0000,  REALM::DHOESONE,                               0,  RUINS_ICON,   0,0,0,0,0,0,0, 0,  0, NULL     , fERROR},
///* xxx */   { 0000,  0000,  REALM::MONSTERS,                               1,  CASTLE1_ICON, 0,0,0,0,0,0,0,64,  0,"tutor"   , fERROR},
/*---------- Sidhuire  (Dhoesone) ----------*/                                               
/* 601 */   { 0000,  0000,  REALM::DHOESONE,                               1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 502 */   { 0000,  0000,  REALM::N_IMPORTS_EXPORTS,                      0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 604 */   { 0000,  0000,  REALM::N_REFORMED_CHURCH_OF_SARIMIE,           0,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 605 */   { 0000,  0000,  REALM::N_IMPORTS_EXPORTS,                      2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 606 */   { 0000,  0000,  REALM::DHOESONE,                               5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Soniele  (Dhoesone) ----------*/                                                
/* 607 */   { 0000,  0000,  REALM::UPPER_ANUIRE_TRADERS,                   1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 608 */   { 0000,  0000,  REALM::DHOESONE,                               0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 610 */   { 0000,  0000,  REALM::UPPER_ANUIRE_TRADERS,                   1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 611 */   { 0000,  0000,  REALM::DHOESONE,                               6,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Sonnelind  (Dhoesone) ----------*/                                              
/* 612 */   { 0000,  0000,  REALM::DHOESONE,                               4,  CASTLE1_ICON, 0,0,0,0,0,1,0,22,  1, NULL     , fERROR},
/* 613 */   { 0000,  0000,  REALM::DHOESONE,                               3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 614 */   { 0000,  0000,  REALM::CARIELE,                                1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 615 */   { 0000,  0000,  REALM::N_REFORMED_CHURCH_OF_SARIMIE,           3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 617 */   { 0000,  0000,  REALM::CARIELE,                                2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 618 */   { 0000,  0000,  REALM::N_IMPORTS_EXPORTS,                      2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  6, NULL     , fERROR},
/* 619 */   { 0000,  0000,  REALM::DHOESONE,                               3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Tradebhein  (Dhoesone) ----------*/                                             
/* 620 */   { 0000,  0000,  REALM::DHOESONE,                               1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 621 */   { 0000,  0000,  REALM::CARIELE,                                0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 622 */   { 0000,  0000,  REALM::N_REFORMED_CHURCH_OF_SARIMIE,           2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 624 */   { 0000,  0000,  REALM::CARIELE,                                2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 625 */   { 0000,  0000,  REALM::DHOESONE,                               5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- GORGON'S CROWN, THE ----------*/                                                
/*---------- Abattoir  (Gorgon's Crown) ----------*/                                         
/* 626 */   { 0000,  0000,  REALM::GORGONSCROWN,                           3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 627 */   { 0000,  0000,  REALM::GORGONSCROWN,                           6,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Anathar  (Gorgon's Crown) ----------*/                                          
/* 628 */   { 1653,    92,  REALM::GORGONSCROWN,                           1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 630 */   { 1660,   174,  REALM::GORGONSCROWN,                           4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Elfseyes  (Gorgon's Crown) ----------*/                                         
/* 631 */   { 0000,  0000,  REALM::GORGONSCROWN,                           2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 632 */   { 0000,  0000,  REALM::GORGONSCROWN,                           7,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Jogh Warren  (Gorgon's Crown) ----------*/                                      
/* 633 */   { 0000,  0000,  REALM::GORGONSCROWN,                           3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 634 */   { 0000,  0000,  REALM::GORGONSCROWN,                           2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 635 */   { 1743,   511,  REALM::GORGONSCROWN,                           0,  SEAPORT2_ICON,0,0,0,0,0,1,0,46,  0, NULL     , fERROR},
/*---------- Kal-Saitharak  (Gorgon's Crown) ----------*/                                    
/* 636 */   { 1500,   348,  REALM::GORGONSCROWN,                           7,  CASTLE4_ICON, 0,0,0,0,0,1,0,23,  1,"fellp_kw", fERROR},
/* 637 */   { 0000,  0000,  REALM::GORGONSCROWN,                           4,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 639 */   { 0000,  0000,  REALM::GORGONSCROWN,                           5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Mettle  (Gorgon's Crown) ----------*/                                           
/* 640 */   { 0000,  0000,  REALM::GORGONSCROWN,                           2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 641 */   { 0000,  0000,  REALM::GORGONSCROWN,                           3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Motile  (Gorgon's Crown) ----------*/                                           
/* 642 */   { 0000,  0000,  REALM::GORGONSCROWN,                           2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 643 */   { 0000,  0000,  REALM::GORGONSCROWN,                           3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Mutian's Point  (Gorgon's Crown) ----------*/                                   
/* 644 */   { 1590,   128,  REALM::GORGONSCROWN,                           1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 646 */   { 1595,    98,  REALM::GORGONSCROWN,                           4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Orog's Head  (Gorgon's Crown) ----------*/                                      
/* 647 */   { 0000,  0000,  REALM::GORGONSCROWN,                           2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 648 */   { 0000,  0000,  REALM::GORGONSCROWN,                           4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Pelt  (Gorgon's Crown) ----------*/                                             
/* 649 */   { 0000,  0000,  REALM::GORGONSCROWN,                           1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 650 */   { 0000,  0000,  REALM::GORGONSCROWN,                           4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Plumbago  (Gorgon's Crown) ----------*/                                         
/* 651 */   { 0000,  0000,  REALM::GORGONSCROWN,                           2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 652 */   { 0000,  0000,  REALM::GORGONSCROWN,                           5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Sage's Fen  (Gorgon's Crown) ----------*/                                       
/* 653 */   { 0000,  0000,  REALM::GORGONSCROWN,                           2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 655 */   { 0000,  0000,  REALM::GORGONSCROWN,                           5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Sere's Hold  (Gorgon's Crown) ----------*/                                      
/* 656 */   { 0000,  0000,  REALM::GORGONSCROWN,                           2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 657 */   { 0000,  0000,  REALM::GORGONSCROWN,                           4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Sideath  (Gorgon's Crown) ----------*/                                          
/* 658 */   { 0000,  0000,  REALM::GORGONSCROWN,                           2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 660 */   { 0000,  0000,  REALM::GORGONSCROWN,                           5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Stone's End  (Gorgon's Crown) ----------*/                                      
/* 661 */   { 0000,  0000,  REALM::GORGONSCROWN,                           1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 663 */   { 0000,  0000,  REALM::GORGONSCROWN,                           4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Sunder Falls  (Gorgon's Crown) ----------*/                                     
/* 664 */   { 0000,  0000,  REALM::GORGONSCROWN,                           1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 666 */   { 0000,  0000,  REALM::GORGONSCROWN,                           3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Zaptig  (Gorgon's Crown) ----------*/                                           
/* 667 */   { 0000,  0000,  REALM::GORGONSCROWN,                           2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 668 */   { 0000,  0000,  REALM::GORGONSCROWN,                           4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- TUARHIEVEL  (ELF) ----------*/                                                  
/*---------- Avallaigh  (Tuarhievel) ----------*/                                            
/* 669 */   { 0000,  0000,  REALM::DHOESONE,                               1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 670 */   { 0000,  0000,  REALM::TUARHIEVEL,                             0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 671 */   { 0000,  0000,  REALM::CARIELE,                                2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 672 */   { 0000,  0000,  REALM::TUARHIEVEL,                             5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Bhindraith  (Tuarhievel) ----------*/                                           
/* 673 */   { 0000,  0000,  REALM::TUARHIEVEL,                             1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 674 */   { 0000,  0000,  REALM::CARIELE,                                2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 675 */   { 0000,  0000,  REALM::TUARHIEVEL,                             0,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 676 */   { 0000,  0000,  REALM::TUARHIEVEL,                             4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Braethindyr  (Tuarhievel) ----------*/                                          
/* 677 */   { 0000,  0000,  REALM::DHOESONE,                               2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 678 */   { 0000,  0000,  REALM::TUARHIEVEL,                             2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 679 */   { 0000,  0000,  REALM::N_IMPORTS_EXPORTS,                      2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 680 */   { 0000,  0000,  REALM::TUARHIEVEL,                             2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 681 */   { 0000,  0000,  REALM::TUARHIEVEL,                             4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Cwmbheir  (Tuarhievel) ----------*/                                             
/* 682 */   { 0000,  0000,  REALM::TUARHIEVEL,                             6,  CASTLE1_ICON, 0,0,0,0,0,1,0,24,  1, NULL/*"thorn_xx"*/, fERROR},
/* 683 */   { 0000,  0000,  REALM::TUARHIEVEL,                             4,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 684 */   { 0000,  0000,  REALM::DHOESONE,                               2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 685 */   { 0000,  0000,  REALM::N_IMPORTS_EXPORTS,                      3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 686 */   { 0000,  0000,  REALM::CARIELE,                                2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 687 */   { 0000,  0000,  REALM::TUARHIEVEL,                             0,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  6, NULL     , fERROR},
/* 688 */   { 0000,  0000,  REALM::TUARHIEVEL,                             5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Cymryr  (Tuarhievel) ----------*/                                          
/* 689 */   { 0000,  0000,  REALM::TUARHIEVEL,                             2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 690 */   { 0000,  0000,  REALM::TUARHIEVEL,                             5,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Dhoneaghmiere  (Tuarhievel) ----------*/                                   
/* 691 */   { 0000,  0000,  REALM::TUARHIEVEL,                             2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 692 */   { 0000,  0000,  REALM::TUARHIEVEL,                             5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Llyrandor  (Tuarhievel) ----------*/                                       
/* 693 */   { 0000,  0000,  REALM::TUARHIEVEL,                             2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 694 */   { 0000,  0000,  REALM::TUARHIEVEL,                             6,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- CARIELE ----------*/                                                       
/*---------- Mhelliviene  (Cariele) ----------*/                                        
/* 695 */   { 0000,  0000,  REALM::CARIELE,                                5,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
///* 696 */   { 0000,  0000,  REALM::CARIELE,                                2,  LAW1_ICON,    0,0,0,0,0,0,1, 0,  2, NULL     , fERROR},
/* 697 */   { 0000,  0000,  REALM::N_REFORMED_CHURCH_OF_SARIMIE,           5,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 698 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          0,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 699 */   { 0000,  0000,  REALM::CARIELE,                                5,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 700 */   { 0000,  0000,  REALM::DHOESONE,                               0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Mountainsedge  (Cariele) ----------*/                                      
/* 701 */   { 0000,  0000,  REALM::CARIELE,                                3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
///* 702 */   { 0000,  0000,  REALM::CARIELE,                                1,  LAW1_ICON,    0,0,0,0,0,0,1, 0,  2, NULL     , fERROR},
/* 703 */   { 0000,  0000,  REALM::N_REFORMED_CHURCH_OF_SARIMIE,           3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 704 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          0,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 705 */   { 0000,  0000,  REALM::CARIELE,                                3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 706 */   { 0000,  0000,  REALM::WIZARD_EYELESS_ONE,                     0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Riverford  (Cariele) ----------*/                                          
/* 707 */   { 0000,  0000,  REALM::CARIELE,                                5,  CASTLE1_ICON, 0,0,0,0,0,1,0,25,  1,"caerl_kw", fERROR},
/* 708 */   { 0000,  0000,  REALM::CARIELE,                                5,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 710 */   { 0000,  0000,  REALM::N_REFORMED_CHURCH_OF_SARIMIE,           5,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 712 */   { 0000,  0000,  REALM::CARIELE,                                5,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 713 */   { 0000,  0000,  REALM::DHOESONE,                               0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 714 */   { 0000,  0000,  REALM::WIZARD_EYELESS_ONE,                     0,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- FIVE PEAKS, THE ----------*/                                               
/*---------- The Gorge  (The Five Peaks) ----------*/                                   
/* 715 */   { 0000,  0000,  REALM::WIZARD_EYELESS_ONE,                     1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Floodspaeth  (The Five Peaks) ----------*/                                 
/*     */   { 0000,  0000,  REALM::FIVEPEAKS,                              0,  RUINS_ICON,   0,0,0,0,0,0,0,50,  1,"barro_mw", fERROR},
/* 716 */   { 0000,  0000,  REALM::WIZARD_EYELESS_ONE,                     1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Helmshaven  (The Five Peaks) ----------*/                                  
/* 718 */   { 0000,  0000,  REALM::WIZARD_EYELESS_ONE,                     1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Puinol  (The Five Peaks) ----------*/                                      
/* 720 */   { 0000,  0000,  REALM::WIZARD_EYELESS_ONE,                     1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Sufhanie  (The Five Peaks) ----------*/                                    
/*     */   { 0000,  0000,  REALM::FIVEPEAKS,                              4,  CASTLE1_ICON, 0,0,0,0,0,1,0,54,  1,"endie_mw", fERROR},
/* 721 */   { 0000,  0000,  REALM::WIZARD_EYELESS_ONE,                     1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Thasbyrn  (The Five Peaks) ----------*/                                    
/* 722 */   { 0000,  0000,  REALM::HIDDEN_TEMPLE_OF_CUIRAECEN,             1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 723 */   { 0000,  0000,  REALM::WIZARD_EYELESS_ONE,                     1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Torain  (The Five Peaks) ----------*/                                      
/* 724 */   { 0000,  0000,  REALM::WIZARD_EYELESS_ONE,                     1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- MARKAZOR  (GOBLIN) ----------*/                                            
/*---------- Brushfire  (Markazor) ----------*/                                         
/* 725 */   { 0000,  0000,  REALM::GORGONSCROWN,                           1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 726 */   { 0000,  0000,  REALM::MARKAZOR,                               1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
///* 728 */   { 0000,  0000,  REALM::APPROVED_TEMPLE_OF_MARKAZOR,          1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 729 */   { 0000,  0000,  REALM::GORGONSCROWN,                           2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 730 */   { 0000,  0000,  REALM::GORGONSCROWN,                           3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Dwarf's Hold  (Markazor) ----------*/                                      
/* 731 */   { 0000,  0000,  REALM::GORGONSCROWN,                           3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 732 */   { 0000,  0000,  REALM::MARKAZOR,                               1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
///* 734 */   { 0000,  0000,  REALM::APPROVED_TEMPLE_OF_MARKAZOR,          2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 735 */   { 0000,  0000,  REALM::GORGONSCROWN,                           4,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 736 */   { 0000,  0000,  REALM::GORGONSCROWN,                           1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Elfsdemise  (Markazor) ----------*/                                        
/* 737 */   { 0000,  0000,  REALM::GORGONSCROWN,                           2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 738 */   { 0000,  0000,  REALM::MARKAZOR,                               1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
///* 740 */   { 0000,  0000,  REALM::APPROVED_TEMPLE_OF_MARKAZOR,          1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 741 */   { 0000,  0000,  REALM::GORGONSCROWN,                           3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 742 */   { 0000,  0000,  REALM::GORGONSCROWN,                           1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Periltrees  (Markazor) ----------*/                                        
/*     */   { 0000,  0000,  REALM::GORGONSCROWN,                           0,  RUINS_ICON,   0,0,0,0,0,1,0,57,  1, NULL/*"ruins_xx"*/, fERROR},
/* 743 */   { 0000,  0000,  REALM::GORGONSCROWN,                           2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 744 */   { 0000,  0000,  REALM::MARKAZOR,                               1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
///* 746 */   { 0000,  0000,  REALM::APPROVED_TEMPLE_OF_MARKAZOR,          1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 747 */   { 0000,  0000,  REALM::GORGONSCROWN,                           3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 748 */   { 0000,  0000,  REALM::GORGONSCROWN,                           2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Riverspring  (Markazor) ----------*/                                       
/* 749 */   { 0000,  0000,  REALM::GORGONSCROWN,                           2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 750 */   { 0000,  0000,  REALM::MARKAZOR,                               1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
///* 752 */   { 0000,  0000,  REALM::APPROVED_TEMPLE_OF_MARKAZOR,          1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 753 */   { 0000,  0000,  REALM::GORGONSCROWN,                           3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 754 */   { 0000,  0000,  REALM::GORGONSCROWN,                           2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Shattered Hills  (Markazor) ----------*/                                   
/* 755 */   { 1183,   753,  REALM::MARKAZOR,                               4,  CASTLE1_ICON, 0,0,0,0,0,1,0,26,  1, NULL /*"calde_xw"*/, fERROR},
/* 756 */   { 0000,  0000,  REALM::GORGONSCROWN,                           2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 757 */   { 0000,  0000,  REALM::MARKAZOR,                               2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
///* 759 */   { 0000,  0000,  REALM::APPROVED_TEMPLE_OF_MARKAZOR,          2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 760 */   { 0000,  0000,  REALM::GORGONSCROWN,                           4,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 761 */   { 0000,  0000,  REALM::GORGONSCROWN,                           1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Sutren Hills  (Markazor) ----------*/                                      
/* 762 */   { 0000,  0000,  REALM::GORGONSCROWN,                           1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 763 */   { 0000,  0000,  REALM::MARKAZOR,                               0,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
///* 765 */   { 0000,  0000,  REALM::APPROVED_TEMPLE_OF_MARKAZOR,          0,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 766 */   { 0000,  0000,  REALM::GORGONSCROWN,                           1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 767 */   { 0000,  0000,  REALM::GORGONSCROWN,                           4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- MUR-KILAD  (DWARF) ----------*/                                            
/*---------- Crushing Rock  (Mur-Kilad) ----------*/                                    
/* 772 */   { 0000,  0000,  REALM::MURKILAD,                               5,  CASTLE1_ICON, 0,0,0,0,0,1,0,27,  1, NULL     , fERROR},
/* 773 */   { 1443,   693,  REALM::GORGONSCROWN,                           3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 774 */   { 1499,   682,  REALM::MURKILAD,                               2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 775 */   { 0000,  0000,  REALM::RUARCH_ROCKHAMMER,                      3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 777 */   { 1463,   671,  REALM::GORGONSCROWN,                           5,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 778 */   { 0000,  0000,  REALM::GORGONSCROWN,                           2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,   0, NULL     , fERROR},
/*---------- Fallen Rock  (Mur-Kilad) ----------*/                                      
/* 780 */   { 1380,   673,  REALM::MURKILAD,                               3,  CASTLE1_ICON, 0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 781 */   { 1411,   680,  REALM::GORGONSCROWN,                           2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 782 */   { 1351,   700,  REALM::MURKILAD,                               1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 783 */   { 0000,  0000,  REALM::RUARCH_ROCKHAMMER,                      2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 785 */   { 0000,  0000,  REALM::GORGONSCROWN,                           3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 786 */   { 0000,  0000,  REALM::GORGONSCROWN,                           4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,   0, NULL     , fERROR},
/*---------- THURAZOR  (GOBLIN) ----------*/                                            
/*---------- Bloodbay  (Thurazor) ----------*/                                          
/* 788 */   { 0000,  0000,  REALM::THURAZOR,                               2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 790 */   { 0000,  0000,  REALM::N_IMPORTS_EXPORTS,                      1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 791 */   { 0000,  0000,  REALM::WIZARD_EYELESS_ONE,                     2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 792 */   { 0000,  0000,  REALM::TALINIE,                                1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Crushing Hills  (Thurazor) ----------*/                                    
/* 793 */   { 0000,  0000,  REALM::THURAZOR,                               3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 795 */   { 0000,  0000,  REALM::UPPER_ANUIRE_TRADERS,                   1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 796 */   { 0000,  0000,  REALM::TALINIE,                                2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Doom's Peak  (Thurazor) ----------*/                                       
/*     */   { 0000,  0000,  REALM::THURAZOR,                               1,  WIZTOWER_ICON,0,0,0,0,0,0,0,52,  1,"doom_mw", fERROR},
/* 797 */   { 0000,  0000,  REALM::THURAZOR,                               2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 799 */   { 0000,  0000,  REALM::N_IMPORTS_EXPORTS,                      1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 800 */   { 0000,  0000,  REALM::WIZARD_EYELESS_ONE,                     2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 801 */   { 0000,  0000,  REALM::TALINIE,                                1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Falling Timber  (Thurazor) ----------*/                                    
/*     */   { 0000,  0000,  REALM::THURAZOR,                               1,  CASTLE1_ICON, 0,0,0,0,0,1,0,62,  1,"sonne_kw", fERROR},
/* 802 */   { 0000,  0000,  REALM::THURAZOR,                               2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 803 */   { 0000,  0000,  REALM::N_IMPORTS_EXPORTS,                      2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 805 */   { 0000,  0000,  REALM::HIDDEN_TEMPLE_OF_CUIRAECEN,             1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 806 */   { 0000,  0000,  REALM::N_IMPORTS_EXPORTS,                      1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 807 */   { 0000,  0000,  REALM::UPPER_ANUIRE_TRADERS,                   1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  6, NULL     , fERROR},
/* 808 */   { 0000,  0000,  REALM::WIZARD_EYELESS_ONE,                     2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Mergarrote  (Thurazor) ----------*/                                        
/* 809 */   { 0000,  0000,  REALM::THURAZOR,                               2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 810 */   { 0000,  0000,  REALM::UPPER_ANUIRE_TRADERS,                   2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 812 */   { 0000,  0000,  REALM::HIDDEN_TEMPLE_OF_CUIRAECEN,             2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 813 */   { 0000,  0000,  REALM::N_IMPORTS_EXPORTS,                      1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 814 */   { 0000,  0000,  REALM::UPPER_ANUIRE_TRADERS,                   1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 815 */   { 0000,  0000,  REALM::WIZARD_EYELESS_ONE,                     3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Storm's Release  (Thurazor) ----------*/                                   
/* 816 */   { 0000,  0000,  REALM::THURAZOR,                               3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 818 */   { 0000,  0000,  REALM::UPPER_ANUIRE_TRADERS,                   1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 819 */   { 0000,  0000,  REALM::WIZARD_EYELESS_ONE,                     2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- BARUK-AZHIK  (DWARF) ----------*/                                          
/*---------- Bran's Retreat  (Baruk-Azhik) ----------*/                                 
/* 820 */   { 0000,  0000,  REALM::BARUKAZHIK,                             4,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 821 */   { 0000,  0000,  REALM::RUARCH_ROCKHAMMER,                      4,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 822 */   { 0000,  0000,  REALM::POINTS_EAST_TRADING_GUILD,              2,  GUILD1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 823 */   { 0000,  0000,  REALM::BARUKAZHIK,                             2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Cliff's Lament  (Baruk-Azhik) ----------*/                                 
/* 824 */   { 0000,  0000,  REALM::BARUKAZHIK,                             4,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 825 */   { 0000,  0000,  REALM::RUARCH_ROCKHAMMER,                      4,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 826 */   { 0000,  0000,  REALM::POINTS_EAST_TRADING_GUILD,              2,  GUILD1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 827 */   { 1634,  1013,  REALM::BARUKAZHIK,                             2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Land's Victory  (Baruk-Azhik) ----------*/                                 
/* 828 */   { 1765,   932,  REALM::BARUKAZHIK,                             3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 829 */   { 0000,  0000,  REALM::RUARCH_ROCKHAMMER,                      3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 830 */   { 1750,   942,  REALM::BARUKAZHIK,                             3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- The Promontory  (Baruk-Azhik) ----------*/                                 
/* 831 */   { 0000,  0000,  REALM::BARUKAZHIK,                             3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 832 */   { 0000,  0000,  REALM::RUARCH_ROCKHAMMER,                      3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 833 */   { 1712,   967,  REALM::BARUKAZHIK,                             3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Rivenrock  (Baruk-Azhik) ----------*/                                      
/* 834 */   { 1629,   863,  REALM::BARUKAZHIK,                             7,  CASTLE1_ICON, 0,0,0,0,0,1,0,28,  1, NULL/*"stone_xx"*/, fERROR},
/* 835 */   { 0000,  0000,  REALM::BARUKAZHIK,                             5,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 836 */   { 0000,  0000,  REALM::RUARCH_ROCKHAMMER,                      5,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 837 */   { 1675,   841,  REALM::MHORIED,                                3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 838 */   { 1656,   882,  REALM::BARUKAZHIK,                             2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- CHIMAERON, THE ----------*/                                                
/*---------- Barniere  (The Chimaeron) ----------*/                                     
/* 839 */   { 0000,  0000,  REALM::COUNCIL_OF_LEADERS,                     1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 840 */   { 0000,  0000,  REALM::CHIMAERON,                              4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Careine  (The Chimaeron) ----------*/                                      
/* 841 */   { 0000,  0000,  REALM::COUNCIL_OF_LEADERS,                     1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 843 */   { 0000,  0000,  REALM::COUNCIL_OF_LEADERS,                     2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 844 */   { 0000,  0000,  REALM::CHIMAERON,                              5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Hamein  (The Chimaeron) ----------*/                                       
/* 845 */   { 0000,  0000,  REALM::COUNCIL_OF_LEADERS,                     1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 847 */   { 1821,  1239,  REALM::COUNCIL_OF_LEADERS,                     1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,   0, NULL     , fERROR},
/*---------- Lyssan  (The Chimaeron) ----------*/                                       
/* 849 */   { 0000,  0000,  REALM::COUNCIL_OF_LEADERS,                     2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 852 */   { 1725,  1200,  REALM::COUNCIL_OF_LEADERS,                     2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 853 */   { 0000,  0000,  REALM::CHIMAERON,                              7,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Mhowe  (The Chimaeron) ----------*/                                        
/* 854 */   { 0000,  0000,  REALM::COUNCIL_OF_LEADERS,                     2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 855 */   { 0000,  0000,  REALM::COUNCIL_OF_LEADERS,                     2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 857 */   { 1854,  1396,  REALM::COUNCIL_OF_LEADERS,                     0,  SEAPORT_ICON, 0,0,0,0,0,1,0,47,  0, NULL     , fERROR},
/*---------- Ruorkhe  (The Chimaeron) ----------*/                                      
/* 858 */   { 0000,  0000,  REALM::COUNCIL_OF_LEADERS,                     1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 859 */   { 0000,  0000,  REALM::COUNCIL_OF_LEADERS,                     1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 860 */   { 0000,  0000,  REALM::CHIMAERON,                              6,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Salviene  (The Chimaeron) ----------*/                                     
/* 861 */   { 0000,  0000,  REALM::COUNCIL_OF_LEADERS,                     1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 862 */   { 0000,  0000,  REALM::CHIMAERON,                              5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- COERANYS ----------*/                                                      
/*---------- Bogsend  (Coeranys) ----------*/                                           
/* 863 */   { 0000,  0000,  REALM::COERANYS,                               1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 864 */   { 0000,  0000,  REALM::COERANYS,                               1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 865 */   { 0000,  0000,  REALM::GHOERE,                                 3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 866 */   { 0000,  0000,  REALM::WIZARD_SECOND_SWAMP_MAGE,               2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Caudraight  (Coeranys) ----------*/                                        
/* 867 */   { 0000,  0000,  REALM::COERANYS,                               4,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
///* 868 */   { 0000,  0000,  REALM::COERANYS,                               2,  LAW1_ICON,    0,0,0,0,0,0,1, 0,  2, NULL     , fERROR},
/* 869 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 870 */   { 0000,  0000,  REALM::GHOERE,                                 2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 871 */   { 0000,  0000,  REALM::BARUKAZHIK,                             2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Deepshadow  (Coeranys) ----------*/                                        
/* 872 */   { 0000,  0000,  REALM::COERANYS,                               2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 873 */   { 0000,  0000,  REALM::COERANYS,                               2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 874 */   { 0000,  0000,  REALM::BARUKAZHIK,                             3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,   0, NULL     , fERROR},
/*---------- Duornil  (Coeranys) ----------*/                                           
/* 876 */   { 0000,  0000,  REALM::COERANYS,                               1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 877 */   { 0000,  0000,  REALM::COERANYS,                               1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 878 */   { 0000,  0000,  REALM::BARUKAZHIK,                             2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,   0, NULL     , fERROR},
/*---------- Mistil  (Coeranys) ----------*/                                            
/* 880 */   { 0000,  0000,  REALM::COERANYS,                               1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 881 */   { 0000,  0000,  REALM::COERANYS,                               2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 882 */   { 0000,  0000,  REALM::BARUKAZHIK,                             2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,   0, NULL     , fERROR},
/*---------- Ranien  (Coeranys) ----------*/                                            
/* 884 */   { 0000,  0000,  REALM::COERANYS,                               1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 885 */   { 0000,  0000,  REALM::COERANYS,                               1,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 886 */   { 0000,  0000,  REALM::POINTS_EAST_TRADING_GUILD,              2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,   0, NULL     , fERROR},
/*---------- Ruorven  (Coeranys) ----------*/                                           
/* 888 */   { 0000,  0000,  REALM::COERANYS,                               4,  CASTLE1_ICON, 0,0,0,0,0,1,0,29,  1,"rourv_mw", fERROR},
/* 889 */   { 0000,  0000,  REALM::COERANYS,                               3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 890 */   { 0000,  0000,  REALM::HAELYNS_AEGIS,                          1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 891 */   { 0000,  0000,  REALM::COERANYS,                               3,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 892 */   { 0000,  0000,  REALM::POINTS_EAST_TRADING_GUILD,              2,  GUILD1_ICON, 0,0,0,0,0,1,0, 0,  5, NULL     , fERROR},
/* 893 */   { 0000,  0000,  REALM::BARUKAZHIK,                             2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  6, NULL     , fERROR},
/* 894 */   { 1540,  1353,  REALM::COERANYS,                               0,  PORT_ICON,    0,0,0,0,0,1,0,48,  0, NULL     , fERROR},
/*---------- OSOERDE ----------*/                                                       
/*---------- Algael  (Osoerde) ----------*/                                             
/* 895 */   { 0000,  0000,  REALM::OSOERDE,                                2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 896 */   { 0000,  0000,  REALM::IMPREGNABLE_HEART_OF_HAELYN,            2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 897 */   { 0000,  0000,  REALM::BARUKAZHIK,                             2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,   0, NULL     , fERROR},
/*---------- Brothendar  (Osoerde) ----------*/                                         
/* 899 */   { 0000,  0000,  REALM::OSOERDE,                                3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 901 */   { 0000,  0000,  REALM::GHOERE,                                 3,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,   0, NULL     , fERROR},
/*---------- Ghalliere  (Osoerde) ----------*/                                          
/* 903 */   { 1316,  1387,  REALM::OSOERDE,                                1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 904 */   { 0000,  0000,  REALM::IMPREGNABLE_HEART_OF_HAELYN,            2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 905 */   { 0000,  0000,  REALM::GHOERE,                                 2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 906 */   { 0000,  0000,  REALM::WIZARD_SECOND_SWAMP_MAGE,               3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Gulfport  (Osoerde) ----------*/                                           
/* 907 */   { 0000,  0000,  REALM::OSOERDE,                                3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 909 */   { 0000,  0000,  REALM::WIZARD_SECOND_SWAMP_MAGE,               2,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Moergen  (Osoerde) ----------*/                                            
///* 910 */   { 0000,  0000,  REALM::DEPOSED_REGENT_OF_OSOERDE,            2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 911 */   { 1351,  1323,  REALM::IMPREGNABLE_HEART_OF_HAELYN,            2,  TEMPLE1_ICON, 0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 912 */   { 0000,  0000,  REALM::BARUKAZHIK,                             2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 913 */   { 0000,  0000,  REALM::WIZARD_SECOND_SWAMP_MAGE,               3,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Moriel  (Osoerde) ----------*/                                             
/* 914 */   { 0000,  0000,  REALM::OSOERDE,                                4,  CASTLE1_ICON, 0,0,0,0,0,0,0,30,  1, NULL     , fERROR},
/* 915 */   { 0000,  0000,  REALM::OSOERDE,                                3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
///* 916 */   { 0000,  0000,  REALM::DEPOSED_REGENT_OF_OSOERDE,            1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 919 */   { 0000,  0000,  REALM::GHOERE,                                 2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  3, NULL     , fERROR},
/* 920 */   { 0000,  0000,  REALM::BARUKAZHIK,                             1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  4, NULL     , fERROR},
/* 921 */   { 0000,  0000,  REALM::WIZARD_SECOND_SWAMP_MAGE,               1,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Spiritsend  (Osoerde) ----------*/                                         
/*     */   { 0000,  0000,  REALM::OSOERDE,                                0,  RUINS_ICON,   0,0,0,0,0,1,0,63,  1,"shadg_tw", fERROR},
/* 922 */   { 0000,  0000,  REALM::OSOERDE,                                1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 923 */   { 0000,  0000,  REALM::BARUKAZHIK,                             2,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,   0, NULL     , fERROR},
/*---------- Sunken Lands  (Osoerde) ----------*/                                       
///* 925 */   { 0000,  0000,  REALM::DEPOSED_REGENT_OF_OSOERDE,            2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 927 */   { 0000,  0000,  REALM::WIZARD_SECOND_SWAMP_MAGE,               6,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- SIELWODE, THE  (ELF) ----------*/                                          
/*---------- Annydwr  (The Sielwode) ----------*/                                       
/* 928 */   { 0000,  0000,  REALM::SIELWODE,                               2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 929 */   { 0000,  0000,  REALM::SIELWODE,                               6,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Cu Haellyrd  (The Sielwode) ----------*/                                   
/* 930 */   { 0000,  0000,  REALM::SIELWODE,                               3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 931 */   { 1670,   779,  REALM::SIELWODE,                               6,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Fhylallien  (The Sielwode) ----------*/                                    
/* 932 */   { 1449,   871,  REALM::SIELWODE,                               6,  CASTLE1_ICON, 0,0,0,0,0,1,0,31,  1, NULL     , fERROR},
/* 933 */   { 0000,  0000,  REALM::SIELWODE,                               6,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 934 */   { 0000,  0000,  REALM::SIELWODE,                               5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Ghyllwn  (The Sielwode) ----------*/                                       
/* 935 */   { 0000,  0000,  REALM::SIELWODE,                               2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 936 */   { 0000,  0000,  REALM::SIELWODE,                               6,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Hoehnaen  (The Sielwode) ----------*/                                      
/* 937 */   { 0000,  0000,  REALM::SIELWODE,                               3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 938 */   { 0000,  0000,  REALM::SIELWODE,                               6,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Iseare  (The Sielwode) ----------*/                                        
/* 939 */   { 0000,  0000,  REALM::SIELWODE,                               4,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 940 */   { 1627,   805,  REALM::SIELWODE,                               5,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Llewhoellen  (The Sielwode) ----------*/                                   
/* 941 */   { 0000,  0000,  REALM::SIELWODE,                               3,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 942 */   { 0000,  0000,  REALM::SIELWODE,                               6,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Tuar-Llyrien  (The Sielwode) ----------*/                                  
/*     */   { 0000,  0000,  REALM::SIELWODE,                               4,  CASTLE1_ICON, 0,0,0,0,0,1,0,55,  1, NULL /*"flyla_gw"*/, fERROR},
/* 943 */   { 0000,  0000,  REALM::SIELWODE,                               2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 944 */   { 0000,  0000,  REALM::SIELWODE,                               6,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Ywrndor  (The Sielwode) ----------*/                                       
/* 945 */   { 0000,  0000,  REALM::SIELWODE,                               2,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 946 */   { 0000,  0000,  REALM::SIELWODE,                               6,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},
/*---------- Lost Realm  (Markazor) ----------*/                                        
/* 768 */   { 0000,  0000,  REALM::GORGONSCROWN,                           1,  LAW1_ICON,    0,0,0,0,0,1,0, 0,  1, NULL     , fERROR},
/* 770 */   { 0000,  0000,  REALM::GORGONSCROWN,                           1,  GUILD1_ICON,  0,0,0,0,0,1,0, 0,  2, NULL     , fERROR},
/* 771 */   { 0000,  0000,  REALM::GORGONSCROWN,                           4,  SOURCE1_ICON, 0,0,0,0,0,1,0, 0,  0, NULL     , fERROR},

};

DEFINE_VECTOR_CLASS(PLACE_TYPE, places);

/*  Place Names  */

DEFINE_VECTOR_DATA(CSTRPTR,place_names) = {

/* 000 */  "Domain Holding",
/* 001 */  "Blacktower Castle",
/* 002 */  "Caer Callin",
/* 003 */  "Aerele",
/* 004 */  "Castle Aglondier",
/* 005 */  "Braeme",          
/* 006 */  "Seaward",         
/* 007 */  "Spider's Lair",   
/* 008 */  "Seaharrow",       
/* 009 */  "Rose Palace",     
/* 010 */  "Bindier",           
/* 011 */  "Tower Ruannoch",    
/* 012 */  "Stormpoint",        
/* 013 */  "Daulton",           
/* 014 */  "Rook's Roost",      
/* 015 */  "Bevaldruor",        
/* 016 */  "Castle Haes",       
/* 017 */  "Lofton",            
/* 018 */  "Sorentier",         
/* 019 */  "City of Anuire",    
/* 020 */  "Ansien",            
/* 021 */  "Falcon's Roost",    
/* 022 */  "Sonnelind",         
/* 023 */  "Kal-Saitharak",     
/* 024 */  "Thorn Throne",      
/* 025 */  "Caerlinien",        
/* 026 */  "Thak Mor Kaden",    
/* 027 */  "Stone Forge",       
/* 028 */  "Stone's Rejoicing", 
/* 029 */  "Ruorven",           
/* 030 */  "Moriel",            
/* 031 */  "Crystal Palace",    
/* 032 */  "Port of Calrie",    
/* 033 */  "Port of Aerele",    
/* 034 */  "Port of Ilien",     
/* 035 */  "Port of Braeme",    
/* 036 */  "Port of Tariene",   
/* 037 */  "Port of Nowelton",  
/* 038 */  "Port of Bindier",   
/* 039 */  "Port of Stormpoint",
/* 040 */  "Port of Ghieste",
/* 041 */  "Port of Riumache",
/* 042 */  "Port of Haes",
/* 043 */  "Port of Lofton",
/* 044 */  "Port of Endier",
/* 045 */  "Port of Nolien",
/* 046 */  "Port of Fellport",
/* 047 */  "Port of Mhowe",
/* 048 */  "Port of Ruorven",
/* 049 */  "Aenier",
/* 050 */  "Barrow Hill",
/* 051 */  "Ortinalae",
/* 052 */  "Malentor's Tower",
/* 053 */  "Brauken Caer",
/* 054 */  "Brak-Nor Castle",
/* 055 */  "Cwymeagh",
/* 056 */  "Gaelin's Keep",
/* 057 */  "Gael's Veil",
/* 058 */  "Barrow Mound",
/* 059 */  "Black Stone",
/* 060 */  "Winterroot",
/* 061 */  "Elfwash Keep",
/* 062 */  "Drak-mal Castle",
/* 063 */  "Spirit's End",
/* 064 */  "Ruidewash Keep",
};

DEFINE_VECTOR_CLASS(CSTRPTR, place_names);

/* ========================================================================
   Function    - IsRelicFound
   Description - has the relic for this scene found
   Returns     - TRUE if already found
   ======================================================================== */
BOOL IsRelicFound( SHORT NameIndex )
{
	BOOL found = FALSE;
	SHORT i;
	
	if (NameIndex == 0)
		return TRUE;	// invalid index
	
	for (i=0; i < MAX_PLACES; i++)
	{
		if (places[i].iName == NameIndex)
		{
			found = TRUE;
			break;
		}
	}
	if (found)
		return places[i].fRelicFound;
	else
		return TRUE;	// you can't get this relic if the place is gone
}


/* ========================================================================
   Function    - SetRelicFound
   Description - set the relic for this scene found
   Returns     - 
   ======================================================================== */
void SetRelicFound( SHORT NameIndex )
{
	SHORT i;
	if (NameIndex == 0)
		return;
		
	for (i=0; i < MAX_PLACES; i++)
	{
		if (places[i].iName == NameIndex)
		{
			places[i].fRelicFound = 1;
			break;
		}
	}
}

/* ========================================================================
   Function    - GetCastleName
   Description - 
   Returns     - return the name of this holding, only castle has name.
   ======================================================================== */
LONG GetCastleName( SHORT PlacesIndex )
{
	return( (LONG) places[PlacesIndex].iName);
}

/*	======================================================================== */


/* ========================================================================
   Function    - 
   Description - 
   Returns     -
   ======================================================================== */
LONG HoldingValue (LONG iHold)
{
	if (iHold < 1 || iHold >= MAX_PLACES || places[iHold].Realm == 0)
		return 0;

	if (places[iHold].Icon == GUILD1_ICON)
		return (places[iHold].Level * 5);		// 5 gb per level for guild
	else
		return (places[iHold].Level * 3);		// 3 gb per level for others
}



