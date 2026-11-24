/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: DLG_EDIT.C
   Author:	 Craig Clayton
   ========================================================================
   Contains the following general functions:
   
   Contains the following internal functions:
   
   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WINDOWS
#include <Windows.h>
#endif

#include "SYSTEM.H"
#include "ENGINE.H"

#include "AVATAR.HXX"
#include "ALIGNMNT.HXX"
#include "DLG_EDIT.H"
#include "GAMEMAP.HXX"
#include "MAIN.HXX"
#include "PLAYSTAT.HXX"
#include "SCENE.HXX"

//extern MAP_INFO gMapInfo; //use access functions
extern long objClicked;
BOOL bEditMode = FALSE;


#ifdef _WINDOWS
#if defined(SCENE_EDITOR)

//#include "winsys\musiutil.h"
#include "resource.h"

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
typedef struct 
{
	char 		ThingName[20];
	THINGTYPE	ThingType;
} AvatarThingTableType;


/* ------------------------------------------------------------------------
	Macros   
   ------------------------------------------------------------------------ */

extern HINSTANCE hInstApp;	// SCENE EDITOR: defined with C++ linkage (MACHINE.C), so no extern "C" here

// These flags determine which controls on the AI dialog box are enabled

#define AI_TARGET_ID    1
#define AI_TARGET_POS   2
#define AI_TARGET_ANGLE 4

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

static void InitEditDialog(
	HWND hwndDlg,
	PTR_SCENE /* pScene */);

static void SaveSceneAvatars(PTR_SCENE /* pScene */);

static void SaveAllAvatars(PTR_SCENE /* pScene */);

static void SaveAvatarData(
		HWND hwndDlg,
		PTR_SCENE /* pScene */ );

static BOOL SaveAvatar(
		HWND hwndDlg,
		PTR_SCENE /* pScene */);

INT_PTR CALLBACK DlgEditorProc(
	HWND  hwndDlg,    // handle to dialog box
	UINT  uMsg,	      // message
	WPARAM  wParam,  // first message parameter
	LPARAM  lParam    // second message parameter
   );

INT_PTR CALLBACK DlgEditAIProc(
	HWND  hwndDlg,    
	UINT  uMsg, 
	WPARAM  wParam,  
	LPARAM  lParam 
   );

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
static long iAvatarAIControlFlags[] = {
	0,											// No AI function
	AI_TARGET_ID  | AI_TARGET_POS,		  // Follow ID
	AI_TARGET_POS | AI_TARGET_ANGLE,      // Guard point
	AI_TARGET_POS,			// Ping pong
};


static AvatarThingTableType AvatarThingTable[] = {
	{"Archer",				ARCHER           },
	{"Light Cavalry",		LIGHT_CAVALRY    },
	{"Elite Infantry",		ELITE_INFANTRY   },
	{"Infantry",			INFANTRY         },
	{"Irregular",			IRREGULAR        },
	{"Heavy Cavalry",		KNIGHT           },
	{"Levy",				LEVY             },
	{"Officer",				OFFICER          },
	{"Pikeman",				PIKEMAN          },
	{"Scout",				SCOUT            },
	{"Troop Hell Hound",	T_HELL_HOUND     },
	{"Troop Harpy",			T_HARPY          },
	{"Troop Skeleton",		T_SKELETON       },
	{"Troop Giant Spider",	T_GIANT_SPIDER   },
	{"Dwarf Archer",		DWARF_ARCHER     },
	{"Dwarf Infantry",		DWARF_INFANTRY   },
	{"Dwarf Officer",		T_DWARF_OFFICER_1    },
	{"Elf Archer",			ELF_ARCHER       },
	{"Elf Cavalry",			ELF_CAVALRY      },
	{"Elf Infantry",		ELF_INFANTRY     },
	{"Elf Officer",			ELF_OFFICER_1      },
	{"Goblin Archer",		GOBLIN_ARCHER    },
	{"Goblin Cavalry",		GOBLIN_CAVALRY   },
	{"Goblin Infantry",		GOBLIN_INFANTRY  },
	{"Mercenary Archer",	MERC_ARCHER      },
	{"Mercenary Cavalry",	MERC_CAVALRY     },
	{"Mercenary Infantry",	MERC_INFANTRY    },
	{"Mercenary Irregular",	MERC_IRREGULAR   },
	{"Mercenary Officer",	MERC_OFFICER     },
	{"Mercenary Pikeman",	MERC_PIKEMAN     },
	{"Gnoll Infantry",		GNOLL_1   },
	{"Gnoll Irregular",		GNOLL_IRREGULAR  },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Ambassador ",			AMBASSADOR       },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Counselor",			COUNSELOR        },
	{"Chief",				CHIEF            },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"General 1",			GENERAL_1        },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"General 2",			GENERAL_2        },
	{"Guard",				GUARD            },
	{"Lieutenant",			LIEUTENANT       },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Merchant",			MERCHANT         },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Trader",				TRADER           },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Old Woman",			OLD_WOMAN        },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Old Man",				OLD_MAN          },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Old Lord",			OLD_LORD         },
	{"High Priest",			PRIEST_MALE_1      },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Monk",				MONK             },
	{"High Priestess",		PRIEST_FEMALE_1   },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Peasant Woman 1",		PEASANT_WOMAN_1  },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Peasant Woman 2",		PEASANT_WOMAN_2  },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Peasant Man 1",		PEASANT_MAN_1    },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Peasant Man 2",		PEASANT_MAN_2    },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Ranger",				RANGER           },
	{"Rogue",				ROGUE            },
	{"Royal Guard",			ROYAL_GUARD      },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Servant Woman",		SERVANT_WOMAN    },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Servant Man",			SERVANT_MAN      },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Shop Keeper 1",		SHOP_KEEPER_1    },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Shop Keeper 2",		SHOP_KEEPER_2    },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Smith",				SMITH            },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Thug",				THUG             },
	{"Warrior",				WARRIOR          },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Wench",				WENCH            },
	{"Wizard Female 1",		WIZARD_FEMALE_1  },
	{"Wizard Female 1",		WIZARD_FEMALE_2  },
	{"Wizard Male 1",		WIZARD_MALE_1    },
	{"Wizard Male 2",		WIZARD_MALE_2    },
	{"Lord Female 1",		LORD_FEMALE_1    },
	{"Lord Female 2",		LORD_FEMALE_2    },
	{"Lord Female 3",		LORD_FEMALE_3    },
	{"Lord Male 1",			LORD_MALE_1      },
	{"Lord Male 2",			LORD_MALE_2      },
	{"Lord Male 3",			LORD_MALE_3      },
	{"Dwarf Lord",			DWARF_LORD_1       },
	{"Dwarf Guard",			DWARF_GUARD      },
	{"Elf Guard",			ELF_GUARD        },
	{"Elf Lord",			ELF_LORD_1         },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Gnoll Guard",			GNOLL_GUARD      },
	{"Goblin Guard",		GOBLIN_GUARD     },
	{"Ankheg",				ANKHEG           },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Bat",					BAT              },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Crow",				CROW             },
	{"Dog",					DOG              },
	{"Gorgon",				GORGON           },
	{"Harpy",				HARPY            },
	{"Hell Hound",			HELL_HOUND       },
	{"Ogre",				OGRE             },
	{"Rhuobhe Manslayer",	MANSLAYER        },
	{"Skeleton",			SKELETON         },
	{"Spectre",				SPECTRE          },
	{"Spider King",			SPIDER_KING      },
	{"Giant Spider",		GIANT_SPIDER     },
	{"Wyvern",				WYVERN_1           }
};

static LONG	NumAvatarThings = sizeof(AvatarThingTable)/sizeof(AvatarThingTableType);	
		
static AvatarIdRef AvatarIDtable[] = {
	{"Archer",				ARCHER           },
	{"Light Cavalry",		LIGHT_CAVALRY    },
	{"Elite Infantry",		ELITE_INFANTRY   },
	{"Infantry",			INFANTRY         },
	{"Irregular",			IRREGULAR        },
	{"Heavy Cavalry",		KNIGHT           },
	{"Levy",				LEVY             },
	{"Officer",				OFFICER          },
	{"Pikeman",				PIKEMAN          },
	{"Scout",				SCOUT            },
	{"Troop Hell Hound",	T_HELL_HOUND     },
	{"Troop Harpy",			T_HARPY          },
	{"Troop Skeleton",		T_SKELETON       },
	{"Troop Giant Spider",	T_GIANT_SPIDER   },
	{"Dwarf Archer",		DWARF_ARCHER     },
	{"Dwarf Infantry",		DWARF_INFANTRY   },
	{"Dwarf Officer",		T_DWARF_OFFICER_1    },
	{"Elf Archer",			ELF_ARCHER       },
	{"Elf Cavalry",			ELF_CAVALRY      },
	{"Elf Infantry",		ELF_INFANTRY     },
	{"Elf Officer",			ELF_OFFICER_1      },
	{"Goblin Archer",		GOBLIN_ARCHER    },
	{"Goblin Cavalry",		GOBLIN_CAVALRY   },
	{"Goblin Infantry",		GOBLIN_INFANTRY  },
	{"Mercenary Archer",	MERC_ARCHER      },
	{"Mercenary Cavalry",	MERC_CAVALRY     },
	{"Mercenary Infantry",	MERC_INFANTRY    },
	{"Mercenary Irregular",	MERC_IRREGULAR   },
	{"Mercenary Officer",	MERC_OFFICER     },
	{"Mercenary Pikeman",	MERC_PIKEMAN     },
	{"Gnoll Infantry",		GNOLL_1   },
	{"Gnoll Irregular",		GNOLL_IRREGULAR  },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Ambassador ",			AMBASSADOR       },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Counselor",			COUNSELOR        },
	{"Chief",				CHIEF            },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"General 1",			GENERAL_1        },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"General 2",			GENERAL_2        },
	{"Guard",				GUARD            },
	{"Lieutenant",			LIEUTENANT       },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Merchant",			MERCHANT         },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Trader",				TRADER           },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Old Woman",			OLD_WOMAN        },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Old Man",				OLD_MAN          },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Old Lord",			OLD_LORD         },
	{"High Priest",			PRIEST_MALE_1      },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Monk",				MONK             },
	{"High Priestess",		PRIEST_FEMALE_1   },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Peasant Woman 1",		PEASANT_WOMAN_1  },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Peasant Woman 2",		PEASANT_WOMAN_2  },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Peasant Man 1",		PEASANT_MAN_1    },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Peasant Man 2",		PEASANT_MAN_2    },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Ranger",				RANGER           },
	{"Rogue",				ROGUE            },
	{"Royal Guard",			ROYAL_GUARD      },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Servant Woman",		SERVANT_WOMAN    },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Servant Man",			SERVANT_MAN      },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Shop Keeper 1",		SHOP_KEEPER_1    },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Shop Keeper 2",		SHOP_KEEPER_2    },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Smith",				SMITH            },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Thug",				THUG             },
	{"Warrior",				WARRIOR          },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Wench",				WENCH            },
	{"Wizard Female 1",		WIZARD_FEMALE_1  },
	{"Wizard Female 1",		WIZARD_FEMALE_2  },
	{"Wizard Male 1",		WIZARD_MALE_1    },
	{"Wizard Male 2",		WIZARD_MALE_2    },
	{"Lord Female 1",		LORD_FEMALE_1    },
	{"Lord Female 2",		LORD_FEMALE_2    },
	{"Lord Female 3",		LORD_FEMALE_3    },
	{"Lord Male 1",			LORD_MALE_1      },
	{"Lord Male 2",			LORD_MALE_2      },
	{"Lord Male 3",			LORD_MALE_3      },
	{"Dwarf Lord",			DWARF_LORD_1       },
	{"Dwarf Guard",			DWARF_GUARD      },
	{"Elf Guard",			ELF_GUARD        },
	{"Elf Lord",			ELF_LORD_1         },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Gnoll Guard",			GNOLL_GUARD      },
	{"Goblin Guard",		GOBLIN_GUARD     },
	{"Ankheg",				ANKHEG           },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Bat",					BAT              },
	// SCENE EDITOR: type no longer exists in THINGTYP.H: {"Crow",				CROW             },
	{"Dog",					DOG              },
	{"Gorgon",				GORGON           },
	{"Harpy",				HARPY            },
	{"Hell Hound",			HELL_HOUND       },
	{"Ogre",				OGRE             },
	{"Rhuobhe Manslayer",	MANSLAYER        },
	{"Skeleton",			SKELETON         },
	{"Spectre",				SPECTRE          },
	{"Spider King",			SPIDER_KING      },
	{"Giant Spider",		GIANT_SPIDER     },
	{"Wyvern",				WYVERN_1           },
	{"Battle Avatar 100", 1100},
	{"Battle Avatar 101", 1101},
	{"Battle Avatar 102", 1102},
	{"Battle Avatar 103", 1103},
	{"Battle Avatar 104", 1104},
	{"Battle Avatar 105", 1105},
	{"Battle Avatar 106", 1106},
	{"Battle Avatar 107", 1107},
	{"Battle Avatar 108", 1108},
	{"Battle Avatar 109", 1109}
}; 

// SCENE EDITOR: was a hard-coded 97; entries for types that no longer exist were removed.
#define NUM_AVATAR_IDS	(sizeof(AvatarIDtable)/sizeof(AvatarIDtable[0]))



// SCENE EDITOR: PLAYER_STATS keeps its data private; PLAYSTAT.HXX makes this
// struct a friend (under SCENE_EDITOR) so the avatar dialog can edit it.
struct SCENE_EDITOR_STATS_ACCESS {
	static ALIGNMENT_INFO::TYPE & Alignment(PLAYER_STATS *p)	{ return p->Alignment; }
	static PLAYER_TITLE::TITLE & Title(PLAYER_STATS *p)			{ return p->TitleClass.fTitle; }
	static PLAYER_CLASS_LIST & Classes(PLAYER_STATS *p)			{ return p->PlayerClassList; }
	static SHORT & HitPts(PLAYER_STATS *p)						{ return p->HitPts; }
	static SHORT & FullHitPts(PLAYER_STATS *p)					{ return p->FullHitPts; }
	static SBYTE & ArmorClass(PLAYER_STATS *p)					{ return p->sbArmorClass; }
	static ABILITY & Ability(PLAYER_STATS *p)					{ return p->Ability; }
	static char * Name(PLAYER_STATS *p)							{ return p->szNameStat; }
};

// Same order as PLAYER_TITLE::TITLE in PLAYID.HXX.
static const char * const szTitleNames[] = {
	"Not Set", "Patriarch", "High Hierophant", "Duchess", "Baroness", "Prince",
	"Overthane", "Guilder", "Wizard", "Count", "Baron", "Emerald Queen",
	"Queen", "Archpriest", "Lord", "Thane"
};
#define NUM_TITLES	(sizeof(szTitleNames)/sizeof(szTitleNames[0]))

static BOOL bThingTypeChanged;

static BOOL bFollowPathMode = FALSE;
static LONG FollowPathIndex;
static AvatarPoint FollowPathAvatarPoints[50];

static LONG iAvatarID;	    // These globals are sent as parameters to the AICreateAvatar routine
static LONG iAvatarIndex;
static THINGTYPE iAvatarThingType;
static LONG iAvatarX;
static LONG iAvatarY;

static LONG iAvatarTargetID;
static LONG iAvatarTargetX;
static LONG iAvatarTargetY;
static LONG iAvatarTargetZ;
static LONG iAvatarTargetA;
static LONG iAvatarAngle;

static ALIGNMENT_INFO::TYPE	iAvatarAlignment;
static PLAYER_TITLE::TITLE	iAvatarTitle;
static PLAYER_CLASS_INFO::TYPE	iAvatarClass;
static CAvatar::AIFUNC 		iAvatarFuncIndex;

static long iWhichAI;	      // Keeps track of which controls should be enables in AI function parameter dialog

// This routine puts up the Avatar editor.  Index = -1 means new Avatar

void DoEditorDialog(
	LONG index,
	LONG x,
	LONG y
	)
{
	if (!bEditMode)
		return;

	if ((bFollowPathMode) && (index == -1))
	{
		FollowPathAvatarPoints[FollowPathIndex].x = x;
		FollowPathAvatarPoints[FollowPathIndex].y = y;
		FollowPathAvatarPoints[FollowPathIndex].z = -58;
		FollowPathAvatarPoints[FollowPathIndex].a = 0;
		FollowPathIndex++;
		return;
	}

	iAvatarIndex = index;

	if (iAvatarIndex == -1)	  // New Avatar
	{
		iAvatarID = 0;			    // Initialize global variables to 0
		iAvatarThingType = NO_THING;
		iAvatarTargetID = 0;
		iAvatarTargetX = 0;
		iAvatarTargetY = 0;
		iAvatarTargetZ = 0;
		iAvatarTargetA = 0;
		iAvatarAngle = 0;
		iAvatarFuncIndex = CAvatar::AI_FUNC_NULL;
	}
	else
	{
		SCENE *pCurrentScene = (SCENE *) BLKPTR(SCENE_MGR::hCurrentScene);
		CAvatar *pAvatar = (CAvatar *) BLKPTR(pCurrentScene->Avatars[iAvatarIndex]);
		
		iAvatarID = pAvatar->Id;  // Initialize global variables to Avatar values
		iAvatarThingType = pAvatar->mfType();
		iAvatarTargetID =  0; //pAvatar->TargetId;
		iAvatarTargetX =  0; //pAvatar->TargetX;
		iAvatarTargetY =  0; //pAvatar->TargetY;
		iAvatarTargetZ = 0;
		iAvatarTargetA = 0;
		iAvatarAngle = pAvatar->mfAngle();
		iAvatarFuncIndex = pAvatar->GetAIFuncIndex();
	}

	iAvatarX = x;
	iAvatarY = y;
	bThingTypeChanged = FALSE;
	DialogBox((HINSTANCE) hInstApp, MAKEINTRESOURCE(IDD_EDITOR), (HWND) NULL, (DLGPROC) DlgEditorProc);
}



INT_PTR CALLBACK DlgEditorProc(
	HWND  hwndDlg,    // handle to dialog box
	UINT  uMsg,	      // message
	WPARAM  wParam,  // first message parameter
	LPARAM    // second message parameter
   )
{
	LONG iCurSel;
	char szText[100];
	PTR_SCENE pCurrentScene = (PTR_SCENE) BLKPTR(SCENE_MGR::hCurrentScene);
	
	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitEditDialog(hwndDlg, pCurrentScene);
	break;

	case WM_COMMAND:
	{
	switch (LOWORD(wParam))
	{
		case IDOK:	// Save to memory
			if (iAvatarIndex == -1)
			{
				iAvatarIndex = CAvatar::CreateAvatar(iAvatarID, 
													 iAvatarThingType, 
													 iAvatarX, iAvatarY, 0, 
													 iAvatarFuncIndex, 
													 pCurrentScene,
													 REALM::NO_COUNTRY);
			}

			SaveAvatarData(hwndDlg, pCurrentScene);
			break;

		case IDC_BUTTON_CREATE_AVATAR:  // Save to memory and disk
			// Create the avatar
			if (iAvatarIndex == -1)
			{
				iAvatarIndex = CAvatar::CreateAvatar(iAvatarID, iAvatarThingType, 
										iAvatarX, iAvatarY, 0, 
										iAvatarFuncIndex, 
										pCurrentScene,
										REALM::NO_COUNTRY);
			}

			if (bFollowPathMode)
			{
				// Last point needs to be set to point 0
				FollowPathAvatarPoints[FollowPathIndex].x = FollowPathAvatarPoints[0].x;
				FollowPathAvatarPoints[FollowPathIndex].y = FollowPathAvatarPoints[0].y;
				FollowPathAvatarPoints[FollowPathIndex].z = FollowPathAvatarPoints[0].z;
				FollowPathAvatarPoints[FollowPathIndex].a = FollowPathAvatarPoints[0].a;
				FollowPathIndex++;

				MessageBox(hwndDlg, "Follow path mode off", "Follow Path", MB_OK);
				bFollowPathMode = FALSE;
//					pAvatar = Avatars[iAvatarIndex];
				// Orphan the memory to make room for a larger block
//					Avatars[iAvatarIndex] = (AVATAR *) (apBlocks[NewLockedBlock(sizeof(AVATAR)+sizeof(AvatarPoint)*(FollowPathIndex))]);
//					memcpy(Avatars[iAvatarIndex], pAvatar, sizeof(AVATAR));
//					memcpy(((char *) Avatars[iAvatarIndex])+sizeof(AVATAR), FollowPathAvatarPoints, sizeof(AvatarPoint)*(FollowPathIndex));
//					Avatars[iAvatarIndex]->PathCount = FollowPathIndex;
//					Avatars[iAvatarIndex]->PathIndex = 0;
//					Avatars[iAvatarIndex]->pPath = (long *) ((char *) Avatars[iAvatarIndex] + sizeof(AVATAR));
			}

			SaveAvatarData(hwndDlg, pCurrentScene);		// Create in memory

			if (SaveAvatar(hwndDlg, pCurrentScene))
			{
				EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON_DELETE_AVATAR), TRUE);
				CAvatar *pAvatar = (CAvatar *) BLKPTR(pCurrentScene->Avatars[iAvatarIndex]);
				sprintf(szText, "Avatar file id%d.ava saved.  You must save the scene before exiting.", pAvatar->Id);
				MessageBox(hwndDlg, szText, "Create", MB_OK);

			}
			else
				MessageBox(hwndDlg, "Error saving Avatar.", "Save", MB_ICONHAND | MB_OK);

			break;

		case IDC_BUTTON_CANCEL:
			EndDialog(hwndDlg, TRUE);
			break;

		case IDC_BUTTON_DELETE_AVATAR:  // Delete the Avatar
			BLKPTR(pCurrentScene->Avatars[iAvatarIndex]);
			CAvatar::DeleteAvatar(pCurrentScene->Avatars[iAvatarIndex]);
			pCurrentScene->Avatars[iAvatarIndex] = fERROR;
			sprintf(szText, "Avatar deleted.  You must delete id%d.ava and save the scene before exiting.", iAvatarID);
			MessageBox(hwndDlg, szText, "Delete", MB_OK);   
			break;

		case IDC_BUTTON_SAVE_SCENE:	      // Must save the scene for the Avatar to appear again in scene
			SaveSceneAvatars(pCurrentScene);
			break;

		case IDC_BUTTON_SAVE_ALL_AVATARS:
			SaveAllAvatars(pCurrentScene);
			break;

		case IDC_LIST_ID:	  // Store current selection
			iAvatarID = AvatarIDtable[SendDlgItemMessage(hwndDlg, IDC_LIST_ID, LB_GETCURSEL, (WPARAM)0, (LPARAM) 0)].AvatarId;
			break;

		case IDC_LIST_THINGTYPE:	// Store current selection
			iAvatarThingType = AvatarThingTable[SendDlgItemMessage(hwndDlg, IDC_LIST_THINGTYPE, LB_GETCURSEL, (WPARAM)0, (LPARAM) 0)].ThingType;
			bThingTypeChanged = TRUE;
			break;

		case IDC_LIST_ALIGNMENT:
			iAvatarAlignment = (ALIGNMENT_INFO::TYPE) SendDlgItemMessage(hwndDlg, IDC_LIST_ALIGNMENT, LB_GETCURSEL, (WPARAM)0, (LPARAM) 0);
			break;

		case IDC_LIST_CLASS:
			iAvatarClass = (PLAYER_CLASS_INFO::TYPE) SendDlgItemMessage(hwndDlg, IDC_LIST_CLASS, LB_GETCURSEL, (WPARAM)0, (LPARAM) 0);
			break;

		case IDC_LIST_TITLE:
			iAvatarTitle = (PLAYER_TITLE::TITLE) SendDlgItemMessage(hwndDlg, IDC_LIST_TITLE, LB_GETCURSEL, (WPARAM)0, (LPARAM) 0);
			break;

		case IDC_LIST_AI:	  // Get the AI routine selected so the AI dialog box can enable controls needed
			if (HIWORD(wParam) == LBN_SELCHANGE)	    // Only do this if selection is changing
			{			  // Get the current selection from the list box
				iCurSel = SendDlgItemMessage(hwndDlg, IDC_LIST_AI, LB_GETCURSEL, (WPARAM)0, (LPARAM) 0);
				iWhichAI = iAvatarAIControlFlags[iCurSel];	// Get which items should be enabled in AI dialog box

				if (iCurSel != 0)	  // If an AI routine is selected, pop up AI dialog box to get more information
				{
					// SCENE EDITOR: CAvatar::AI_FUNC_GUARD_POINT no longer exists, so the
					// "default the guard point to the avatar's position" step is gone.

					if (DialogBox((HINSTANCE) hInstApp, MAKEINTRESOURCE(IDD_EDIT_AI), hwndDlg, (DLGPROC) DlgEditAIProc))
					{
//							pAvatarAI = AI_functions[iCurSel];
						iAvatarFuncIndex = (CAvatar::AIFUNC)iCurSel;

						if (iAvatarFuncIndex == CAvatar::AI_FUNC_FOLLOW_PATH)
						{
							MessageBox(hwndDlg, "Save Avatar to memory, exit, click points on map, then right click on avatar and save to disk", "Follow Path", MB_OK);
							FollowPathIndex = 0;
							FollowPathAvatarPoints[FollowPathIndex].x = iAvatarX;
							FollowPathAvatarPoints[FollowPathIndex].y = iAvatarY;
							FollowPathAvatarPoints[FollowPathIndex].z = -58;
							FollowPathAvatarPoints[FollowPathIndex].a = iAvatarAngle;
							FollowPathIndex++;
							bFollowPathMode = TRUE;
						}
					}
					else	    // Remove the selection
						SendDlgItemMessage(hwndDlg, IDC_LIST_AI, LB_SETCURSEL, (WPARAM) -1, (LPARAM) 0);
				}
			}
			break;

		default: 
			break ;
		}

	}

	default: break;
	}

	return FALSE ;

}


static void InitEditDialog(
	HWND hwndDlg,
	PTR_SCENE pScene)
{
	HWND hwndCtrl;
	char szTemp[10];
	LONG i;

	for (i=0; i<NUM_AVATAR_IDS; i++)
		SendDlgItemMessage(hwndDlg, IDC_LIST_ID, LB_ADDSTRING, (WPARAM)0, (LPARAM) AvatarIDtable[i].AvatarName);

	for (i=0; i<NumAvatarThings; i++)
		SendDlgItemMessage(hwndDlg, IDC_LIST_THINGTYPE, LB_ADDSTRING, (WPARAM)0, (LPARAM) AvatarThingTable[i].ThingName);

	for (i=0; i<ALIGNMENT_INFO::MAX_ALIGNMENTS; i++)
		SendDlgItemMessage(hwndDlg, IDC_LIST_ALIGNMENT, LB_ADDSTRING, (WPARAM)0, (LPARAM) ALIGNMENT_INFO::mfGetName((ALIGNMENT_INFO::TYPE) i));

	for (i=0; i<PLAYER_CLASS_INFO::CLASS_MAX_NUMBER; i++)
		SendDlgItemMessage(hwndDlg, IDC_LIST_CLASS, LB_ADDSTRING, (WPARAM)0, (LPARAM) PLAYER_CLASS_INFO::mfGetName((PLAYER_CLASS_INFO::TYPE) i));

	// SCENE EDITOR: the title name table only exists in the character editor
	// (_CHARED), so list the titles by the enum names in PLAYID.HXX.
	for (i=0; i<(LONG)NUM_TITLES; i++)
		SendDlgItemMessage(hwndDlg, IDC_LIST_TITLE, LB_ADDSTRING, (WPARAM)0, (LPARAM) szTitleNames[i]);

	SendDlgItemMessage(hwndDlg, IDC_LIST_RACE, LB_ADDSTRING, (WPARAM)0, (LPARAM) "Dwarf");
	SendDlgItemMessage(hwndDlg, IDC_LIST_RACE, LB_ADDSTRING, (WPARAM)0, (LPARAM) "Elf");
	SendDlgItemMessage(hwndDlg, IDC_LIST_RACE, LB_ADDSTRING, (WPARAM)0, (LPARAM) "Half Elf");
	SendDlgItemMessage(hwndDlg, IDC_LIST_RACE, LB_ADDSTRING, (WPARAM)0, (LPARAM) "Halfling");
	SendDlgItemMessage(hwndDlg, IDC_LIST_RACE, LB_ADDSTRING, (WPARAM)0, (LPARAM) "Human Anuirean");
	SendDlgItemMessage(hwndDlg, IDC_LIST_RACE, LB_ADDSTRING, (WPARAM)0, (LPARAM) "Human Brecht");
	SendDlgItemMessage(hwndDlg, IDC_LIST_RACE, LB_ADDSTRING, (WPARAM)0, (LPARAM) "Human Khinasi");
	SendDlgItemMessage(hwndDlg, IDC_LIST_RACE, LB_ADDSTRING, (WPARAM)0, (LPARAM) "Human Rjurik");
	SendDlgItemMessage(hwndDlg, IDC_LIST_RACE, LB_ADDSTRING, (WPARAM)0, (LPARAM) "Human Vos");

		// The order of these AI routines needs to be the same as the AIFunc array
	SendDlgItemMessage(hwndDlg, IDC_LIST_AI, LB_ADDSTRING, (WPARAM)0, (LPARAM) "None");
	SendDlgItemMessage(hwndDlg, IDC_LIST_AI, LB_ADDSTRING, (WPARAM)0, (LPARAM) "Follow Path");
	SendDlgItemMessage(hwndDlg, IDC_LIST_AI, LB_ADDSTRING, (WPARAM)0, (LPARAM) "Follow ID");
	SendDlgItemMessage(hwndDlg, IDC_LIST_AI, LB_ADDSTRING, (WPARAM)0, (LPARAM) "Guard Point");
	SendDlgItemMessage(hwndDlg, IDC_LIST_AI, LB_ADDSTRING, (WPARAM)0, (LPARAM) "Ping Pong");
	SendDlgItemMessage(hwndDlg, IDC_LIST_AI, LB_ADDSTRING, (WPARAM)0, (LPARAM) "Battle Captain");

	if (iAvatarIndex != -1)
	{
		CAvatar *pAvatar = (CAvatar *) BLKPTR(pScene->Avatars[iAvatarIndex]);
		// Put the ID in static text 
		hwndCtrl = GetDlgItem(hwndDlg, IDC_STATIC_ID);
		itoa(iAvatarID, szTemp, 10);
		SetWindowText(hwndCtrl, szTemp);

		// Set the thing ID
		for (i=0; i<NumAvatarThings; i++)
			if (AvatarThingTable[i].ThingType == iAvatarThingType)
				SendDlgItemMessage(hwndDlg, IDC_LIST_THINGTYPE, LB_SETCURSEL, (WPARAM) i, (LPARAM) 0);

		// Set the ID
		for (i=0; i<NUM_AVATAR_IDS; i++)
			if (AvatarIDtable[i].AvatarId == iAvatarID)
				SendDlgItemMessage(hwndDlg, IDC_LIST_ID, LB_SETCURSEL, (WPARAM) i, (LPARAM) 0);

		// Set the AI routine
		SendDlgItemMessage(hwndDlg, IDC_LIST_AI, LB_SETCURSEL, (WPARAM) pAvatar->GetAIFuncIndex(), (LPARAM) 0);

		// Set the angle
		hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_ANGLE);
		itoa(pAvatar->mfAngle(), szTemp, 10);
		SetWindowText(hwndCtrl, szTemp);

		if (pAvatar->hPlayerStats != fERROR)
		{
			// Set the alignment
			PTR_PLAYER_STATS pPlayerStats = (PTR_PLAYER_STATS) BLKPTR(pAvatar->hPlayerStats);
			SendDlgItemMessage(hwndDlg, IDC_LIST_ALIGNMENT, LB_SETCURSEL, (WPARAM) pPlayerStats->mfGetAlignment(), (LPARAM) 0);
	
			// Set the class
			SendDlgItemMessage(hwndDlg, IDC_LIST_CLASS, LB_SETCURSEL, (WPARAM) pPlayerStats->mfGetClassType(0), (LPARAM) 0);
	
			// Set the level
			hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_LEVEL);
			itoa(pPlayerStats->mfGetLevel(0), szTemp, 10);
			SetWindowText(hwndCtrl, szTemp);
	
			// Set the hit points
			hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_HP);
			itoa(pPlayerStats->mfGetMaxHitPoints(), szTemp, 10);
			SetWindowText(hwndCtrl, szTemp);
	
			// Set the armor class
			hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_AC);
			itoa(pPlayerStats->mfGetArmorClass(), szTemp, 10);
			SetWindowText(hwndCtrl, szTemp);
	
			// Set the primary 6 characteristics
			hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_STRENGTH);
			itoa(pPlayerStats->mfGetStr(), szTemp, 10);
			SetWindowText(hwndCtrl, szTemp);
			hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_DEXTERITY);
			itoa(pPlayerStats->mfGetDex(), szTemp, 10);
			SetWindowText(hwndCtrl, szTemp);
			hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_CONSTITUTION);
			itoa(pPlayerStats->mfGetCon(), szTemp, 10);
			SetWindowText(hwndCtrl, szTemp);
			hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_INTELLIGENCE);
			itoa(pPlayerStats->mfGetInt(), szTemp, 10);
			SetWindowText(hwndCtrl, szTemp);
			hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_WISDOM);
			itoa(pPlayerStats->mfGetWis(), szTemp, 10);
			SetWindowText(hwndCtrl, szTemp);
			hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_CHARISMA);
			itoa(pPlayerStats->mfGetChr(), szTemp, 10);
			SetWindowText(hwndCtrl, szTemp);
	
			// Set the name
			hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_NAME);
			SetWindowText(hwndCtrl, pPlayerStats->mfGetName());
	
			// Set the title
			SendDlgItemMessage(hwndDlg, IDC_LIST_TITLE, LB_SETCURSEL, (WPARAM) SCENE_EDITOR_STATS_ACCESS::Title(pPlayerStats), (LPARAM) 0);
		}
	}
	else	    // Or just put dummy values in
	{
		hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_ANGLE);
		itoa(iAvatarAngle, szTemp, 10);
		SetWindowText(hwndCtrl, szTemp);

		hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_STRENGTH);
		SetWindowText(hwndCtrl, "3");
		hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_DEXTERITY);
		SetWindowText(hwndCtrl, "3");
		hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_CONSTITUTION);
		SetWindowText(hwndCtrl, "3");
		hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_INTELLIGENCE);
		SetWindowText(hwndCtrl, "3");
		hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_WISDOM);
		SetWindowText(hwndCtrl, "3");
		hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_CHARISMA);
		SetWindowText(hwndCtrl, "3");
	}

	if (iAvatarIndex != -1)	  // If Avatar exists, can't create one
		EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON_DELETE_AVATAR), TRUE);

//#if !OLDSTYLE
//	EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON_SAVE_ALL_AVATARS), FALSE);
//#endif
}


INT_PTR CALLBACK DlgEditAIProc(
	HWND  hwndDlg,    // handle to dialog box
	UINT  uMsg,	      // message
	WPARAM  wParam,  // first message parameter
	LPARAM    // second message parameter
   )
{
	HWND hwndCtrl;
	char szTemp[10];

	switch (uMsg)
	{
		case WM_INITDIALOG:
			SendDlgItemMessage(hwndDlg, IDC_EDIT_TARGETID, WM_ENABLE, (WPARAM) iWhichAI & AI_TARGET_ID, (LPARAM) 0);
			SendDlgItemMessage(hwndDlg, IDC_EDIT_TARGETX, WM_ENABLE, (WPARAM) iWhichAI & AI_TARGET_POS, (LPARAM) 0);
			SendDlgItemMessage(hwndDlg, IDC_EDIT_TARGETY, WM_ENABLE, (WPARAM) iWhichAI & AI_TARGET_POS, (LPARAM) 0);
// Not used		      SendDlgItemMessage(hwndDlg, IDC_EDIT_TARGETZ, WM_ENABLE, (WPARAM) iWhichAI & AI_TARGET_POS, (LPARAM) 0);
			SendDlgItemMessage(hwndDlg, IDC_EDIT_TARGETA, WM_ENABLE, (WPARAM) iWhichAI & AI_TARGET_ANGLE, (LPARAM) 0);

			hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_TARGETID);
			SetWindowText(hwndCtrl, "0");
			hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_TARGETX);

			itoa(iAvatarTargetX, szTemp, 10);
			SetWindowText(hwndCtrl, szTemp);
			hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_TARGETY);
			itoa(iAvatarTargetY, szTemp, 10);
			SetWindowText(hwndCtrl, szTemp);
			hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_TARGETZ);
			SetWindowText(hwndCtrl, "0");
			hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_TARGETA);
			SetWindowText(hwndCtrl, "0");
		break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDOK:
					GetDlgItemText(hwndDlg, IDC_EDIT_TARGETID, szTemp, 10);
					iAvatarTargetID = atoi(szTemp);
					GetDlgItemText(hwndDlg, IDC_EDIT_TARGETX, szTemp, 10);
					iAvatarTargetX = atoi(szTemp);
					GetDlgItemText(hwndDlg, IDC_EDIT_TARGETY, szTemp, 10);
					iAvatarTargetY = atoi(szTemp);
//					GetDlgItemText(hwndDlg, IDC_EDIT_TARGETZ, szTemp, 10);  // Not used
//					iAvatarTargetZ = atoi(szTemp);
					GetDlgItemText(hwndDlg, IDC_EDIT_TARGETA, szTemp, 10);
					iAvatarTargetA = atoi(szTemp);

					EndDialog(hwndDlg, TRUE);
					break;

				case IDCANCEL:
					EndDialog(hwndDlg, FALSE);
					break;

				default: break;
			}
		}
		default: break;
	}
	return FALSE;
}


// This routine is only called by the Avatar editor.  
// It creates an Avatar and places it in the array

static void SaveAvatarData(
		HWND hwndDlg,
		PTR_SCENE pScene)
{
	char szTemp[10];
	HWND hwndCtrl;
	CAvatar *pAvatar = (CAvatar *) BLKPTR(pScene->Avatars[iAvatarIndex]);
	
	// Get the angle
	GetDlgItemText(hwndDlg, IDC_EDIT_ANGLE, szTemp, 10);    
	iAvatarAngle = atoi(szTemp);


	// Use data from AI dialog box and set additional values (this should move out of here)
	// GWP pAvatar->TargetId = iAvatarTargetID;
	// GWP pAvatar->TargetX = iAvatarTargetX;
	// GWP pAvatar->TargetY = iAvatarTargetY;
	// GWP pAvatar->TargetZ = iAvatarTargetX;	      // Not used
	// GWP pAvatar->TargetA = iAvatarTargetA;
	pAvatar->SetAIFuncIndex(iAvatarFuncIndex);

	pAvatar->Status = CAvatar::AI_INIT;		// Always init at first
	// Now Stand still.
	pAvatar->DoAI(CAvatar::AI_BEGIN_PAUSE);

	// SCENE EDITOR: the old two-argument load_obj_graphic no longer exists;
	// pAvatar->mfSetType() below calls ChangeThingType(), which reloads the graphic.

	pAvatar->mfSetType(iAvatarThingType);
	pAvatar->mfSetAngle(iAvatarAngle);

	if (pAvatar->hPlayerStats == fERROR)
	{
		pAvatar->hPlayerStats = PLAYER_STATS::mfAllocateSpace(pAvatar->hThis, pAvatar->mfType());
	}
		
	if (pAvatar->hPlayerStats != fERROR)
	{
		PTR_PLAYER_STATS pPlayerStats = (PTR_PLAYER_STATS) BLKPTR(pAvatar->hPlayerStats);
		// Now set player characteristics
	
		// Set the alignment
		SCENE_EDITOR_STATS_ACCESS::Alignment(pPlayerStats) = iAvatarAlignment;
	
		// Set the title
		SCENE_EDITOR_STATS_ACCESS::Title(pPlayerStats) = iAvatarTitle;
	
		// Set the class
		SCENE_EDITOR_STATS_ACCESS::Classes(pPlayerStats).Class[0].Type = iAvatarClass;
	
		// Set the level
		hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_LEVEL);
		GetWindowText(hwndCtrl, szTemp, 10);
		SCENE_EDITOR_STATS_ACCESS::Classes(pPlayerStats).Class[0].sbLevel = (SBYTE)atoi(szTemp);
	
		// Set the hit points
		hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_HP);
		GetWindowText(hwndCtrl, szTemp, 10);
		SCENE_EDITOR_STATS_ACCESS::HitPts(pPlayerStats) = SCENE_EDITOR_STATS_ACCESS::FullHitPts(pPlayerStats) = (SHORT)atoi(szTemp);
	
		// Set the armor class
		hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_AC);
		GetWindowText(hwndCtrl, szTemp, 10);
		SCENE_EDITOR_STATS_ACCESS::ArmorClass(pPlayerStats) = (SBYTE)atoi(szTemp);
	
		// Set the primary 6 characteristics
		hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_STRENGTH);
		GetWindowText(hwndCtrl, szTemp, 10);
		SCENE_EDITOR_STATS_ACCESS::Ability(pPlayerStats).mfSetStrength((SBYTE)atoi(szTemp));
		hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_DEXTERITY);
		GetWindowText(hwndCtrl, szTemp, 10);
		SCENE_EDITOR_STATS_ACCESS::Ability(pPlayerStats).mfSetDexterity((SBYTE)atoi(szTemp));
		hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_CONSTITUTION);
		GetWindowText(hwndCtrl, szTemp, 10);
		SCENE_EDITOR_STATS_ACCESS::Ability(pPlayerStats).mfSetConstitution((SBYTE)atoi(szTemp));
		hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_INTELLIGENCE);
		GetWindowText(hwndCtrl, szTemp, 10);
		SCENE_EDITOR_STATS_ACCESS::Ability(pPlayerStats).mfSetIntelligence((SBYTE)atoi(szTemp));
		hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_WISDOM);
		GetWindowText(hwndCtrl, szTemp, 10);
		SCENE_EDITOR_STATS_ACCESS::Ability(pPlayerStats).mfSetWisdom((SBYTE)atoi(szTemp));
		hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_CHARISMA);
		GetWindowText(hwndCtrl, szTemp, 10);
		SCENE_EDITOR_STATS_ACCESS::Ability(pPlayerStats).mfSetCharisma((SBYTE)atoi(szTemp));
	
		// Set the name
		hwndCtrl = GetDlgItem(hwndDlg, IDC_EDIT_NAME);
		GetWindowText(hwndCtrl, SCENE_EDITOR_STATS_ACCESS::Name(pPlayerStats), MAX_NAME_LEN + 1);
	}
}


// This routine is only called by the Avatar editor.  It saves an Avatar to disk using the ID as the name

static BOOL SaveAvatar(
		HWND,
		PTR_SCENE pScene)
{

	CAvatar *pAvatar = (CAvatar *) BLKPTR(pScene->Avatars[iAvatarIndex]);
	pAvatar->Save();
	return (TRUE);
}


// Only called by the Avatar editor->  This saves a scene.  Avatar IDs and locations are saved

static void SaveSceneAvatars(PTR_SCENE pScene)
{
	BOOL bFileSave = pScene->mfWriteSceneFile();
	char cpMsg[80];
	

	if (bFileSave == FALSE)
	{
		sprintf(cpMsg, "Cannot save scene file scenes\\%s.scn", pScene->mfGetSceneName());
		MessageBox(NULL, cpMsg, "Ok", MB_ICONHAND | MB_OK);
		return;
	}

	sprintf(cpMsg, "Scene saved in scenes\\%s.scn", pScene->mfGetSceneName());
	MessageBox(NULL, cpMsg, "Save", MB_OK); 
}


// Only used by the Avatar editor.  Saves all Avatars.  A different file is created for each avatar.
// Used to save Avatars while in the OLDSTYLE mode of hard coding each Avatar

static void SaveAllAvatars(PTR_SCENE pScene)
{
	LONG i;

	for(i=0; i<MAX_AVATARS; i++)
	{
		if (pScene->Avatars[i] != fERROR)
		{
			CAvatar *pAvatar = (CAvatar *) BLKPTR(pScene->Avatars[i]);
			if (pAvatar != NULL )
			{
				pAvatar->Save();
			}
		}
	}

	MessageBox(NULL, "All Avatars saved", "Save", MB_OK);   
}

#endif
#endif


// Everything after this point is also used in the DOS version.
// So no more Windows calls!

// Set the edit mode flag

void SetEditmode(long)
{
	
	// If there is no scene file, you can't edit it!
	if (SCENE_MGR::hCurrentScene != fERROR)
	{
		static PFVLLL pfPreviousObjClickFunc = 0;
		static PFVLLL pfPreviousFloorClickFunc = 0;
	
		bEditMode = (bEditMode == TRUE) ? FALSE : TRUE;
		// TODO: (fire lizard) uncomment
		//if (bEditMode && !MapIsActive())
		//	ToggleMap(0L);
	
		if (bEditMode)
		{
			printf("Edit mode ON\n");
			
			// Stop the Scene AI.
			SCENE_MGR::mfSetScenePause(TRUE);
			
			push_regions();
			// Add back the required keys for editing
			add_key(KEY_C, PlayerClimbing, -1, -1); // toggle climbing
			add_key(KEY_E, (PFVLL)SetEditmode, 0, 0);
			add_key(KEY_OTHERMINUS,ZoomMap,-1,-1);
			add_key(KEY_OTHERPLUS,ZoomMap,+1,+1);
			
			pfPreviousObjClickFunc = SetMouseClicked(iOBJECT, DoMapFunc);
			pfPreviousFloorClickFunc = SetMouseClicked(iFLOOR, DoMapFunc);
		}
		else
		{
			printf("Edit mode OFF\n");
			
			// Let the scene AI start running again.
			SCENE_MGR::mfSetScenePause(FALSE);
			
			// Restore the hot keys to the previous state.
			pop_regions();
			
			// Get rid of unhandled pending key strokes.
			clear_key_status(0);
			
			// Restore the mouse click fn.
			SetMouseClicked(iOBJECT, pfPreviousObjClickFunc);
			SetMouseClicked(iFLOOR, pfPreviousFloorClickFunc);
		}
	}
}



void DoMapFunc(LONG whichButton , LONG ObjectClicked , LONG )
{

//	printf("mouse:%d X:%d Y:%d", whichMouse, x, y);

#ifdef _WINDOWS
#if defined(SCENE_EDITOR)
	
	if (whichButton == 1)	// Left mouse click
	{
		POINT tp;
		
		tp.x = 0;
		tp.y = -30;
		
		rot8(&tp);
		
		// Put him right in front of the camera.
		DoEditorDialog(-1,PLAYER_INT_VAL(player.x) + tp.x, PLAYER_INT_VAL(player.y) - tp.y);
	}
	else
	if (whichButton == 2)	// Right mouse click
	{
		if(ObjectClicked != fERROR)
		{
			LONG i;
			SCENE *pCurrentScene = (SCENE *) BLKPTR(SCENE_MGR::hCurrentScene);
			
			for(i=0; i<MAX_AVATARS; i++)
				if (pCurrentScene->Avatars[i] != fERROR)
				{
					CAvatar *pAvatar = (CAvatar *) BLKPTR(pCurrentScene->Avatars[i]);
					if (pAvatar->ThingIndex == ObjectClicked)
					{
						DoEditorDialog(i, 
						               pAvatar->mfX(), 
						               pAvatar->mfY());
					}
				}
		}
	}
#endif
#endif
}
