/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _MULTIMAP_HXX
#define _MULTIMAP_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
// extern REALM_STRUCT realm[REALM::REALM_COUNT];
// extern REGENT	regents[];
// extern PROV_TYPE province[];
// extern PLACE_TYPE places[MAX_PLACES];
// extern UNIT_TYPE units[MAX_UNITS];
// extern UBYTE	RealmReaction[LAND_REALM_COUNT][LAND_REALM_COUNT];
// extern UBYTE	RealmStatus[LAND_REALM_COUNT][LAND_REALM_COUNT];

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
BOOL SetGameData(LONG lArray_Id, LONG lStruct_Id, LONG lArray_Index, LONG lData, BOOL fSend );
LONG GetGameData(LONG lArray_Id, LONG lStruct_Id, LONG lArray_Index);
BOOL IsMulPlayerandRealm(int realm);
LONG WaitForSemiphore(LONG iWhichSemiphore);
void SendMyMods(LONG iWhichAction, LONG iWhichProvince, LONG iWhichHold, LONG modRegency);
void SendResult(LONG iWhichAction, LONG iWhichProvince, LONG iWhichHolding, LONG iResult);
LONG WaitForResult(LONG iWhichAction, LONG iWhichProvince, LONG iWhichHolding);
LONG AskAndWaitForTargetPlayer(LONG iWhichAction, LONG iWhichProvince, LONG iWhichHolding, LONG iSuccess, LONG x, LONG y);
void DisplayOthersAction (LONG dwFrom, LONG lAction, LONG lProv, LONG lHold, LONG success);
void RequestPlacesIndex(LONG *index, BOOL fSend);
void DoneHelpKeys(LONG, LONG);
void HandleMultiPlayerEvents(void);

/* ------------------------------------------------------------------------
   Internal Prototypes - don't call these directly
   ------------------------------------------------------------------------ */
void DoSetGameData(LONG lArray_Id, LONG lStruct_Id, LONG lArray_Index, LONG lData);
LONG DoGetGameData(LONG lArray_Id, LONG lStruct_Id, LONG lArray_Index);


/* ------------------------------------------------------------------------
   Structs
   ------------------------------------------------------------------------ */


typedef struct {
	CSTRPTR name;
	SHORT	difficulty;
	BOOL	available;
	LONG	realm;
	SHORT	iPlaces;		// index to matching Places array entry
} ADV_SITE;

//the "available" field in the above structure isn't really a bool
//it uses these codes.
#define AVAILABLE 0
#define SELECTABLE 1
#define DISALLOWED -1

#define MAX_SITES      26
#define MAX_USER_SITES 24
#define MAX_ADV_SITES  (MAX_SITES+MAX_USER_SITES)


/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
typedef enum
{
	MP_REALM,
	MP_REGENT,
	MP_PROVINCE,
	MP_PLACES,
	MP_UNITS,
	MP_REALMREACTION,
	MP_REALMSTATUS,
	MP_GAMEDATA,
	MP_ADVSITE,
	MP_TRIBUTE,

	MP_MAX_TYPES,
} MPARRAY_TYPE;

typedef enum
{
	/* CSTRPTR */  MPADV_NAME,
	/* SHORT   */  MPADV_DIFFICULTY,
	/* BOOL	   */  MPADV_AVAILABLE,

	MPADV_MAX_DATA,
} MPADVSITE_DATA;

typedef enum
{
	/* CSTRPTR		*/	MPRLM_NAME,
	/* SHORT		*/	MPRLM_X,
	/* SHORT		*/	MPRLM_Y,
	/* unsigned int	*/	MPRLM_TITLESIZE,
	/* unsigned int	*/	MPRLM_TITLESPREAD,
	/* unsigned int	*/	MPRLM_TITLETRANS,
	/* unsigned int	*/	MPRLM_REGENT,
//	/* unsigned int	*/	MPRLM_COLOR1,
//	/* unsigned int	*/	MPRLM_COLOR2,
	/* unsigned int	*/	MPRLM_SHOWUNITS,
	/* unsigned int	*/	MPRLM_SHOWHOLDINGS,
	/* unsigned int	*/	MPRLM_PLAYABLE,
	/* unsigned int	*/	MPRLM_PLAYERCTRL,
	/* unsigned int	*/	MPRLM_REGENCY,
	/* unsigned int	*/	MPRLM_TREASURY,
	/* unsigned int	*/	MPRLM_COURT,
	/* unsigned int	*/	MPRLM_FIRST_PROVINCE,
	/* SHORT    	*/	MPRLM_INCOME,
							MPRLM_BATTLEQUERY,
	MPRLM_MAX_DATA,
} MPREALM_DATA;

typedef enum
{
	/* CSTRPTR		*/	MPREG_NAME,
	/* unsigned int	*/	MPREG_REALM,
	/* unsigned int	*/	MPREG_RACE,
	/* unsigned int	*/	MPREG_CLASS1,
	/* unsigned int	*/	MPREG_LEVEL1,
	/* unsigned int	*/	MPREG_CLASS2,
	/* unsigned int	*/	MPREG_LEVEL2,
	/* unsigned int	*/	MPREG_BL_GRADE,
	/* unsigned int	*/	MPREG_BL_DERIV,
	/* unsigned int	*/	MPREG_BL_STRENGTH,
	/* SHORT		*/	MPREG_UNIT,
	/* SHORT        */  MPREG_ID,
	/* SHORT		*/	MPREG_ICON,

	MPREG_MAX_DATA,
} MPREGENT_DATA;

typedef enum
{
	/* CSTRPTR		*/	MPPROV_NAME,
	/* SHORT		*/	MPPROV_X,
	/* SHORT		*/	MPPROV_Y,
	/* unsigned int	*/	MPPROV_REALM,
	/* unsigned int	*/	MPPROV_CIVLEVEL,
	/* unsigned int	*/	MPPROV_CURCIVLEVEL,
	/* unsigned int	*/	MPPROV_MAGLEVEL,
	/* unsigned int	*/	MPPROV_TAXLEVEL,
	/* unsigned int	*/	MPPROV_LOYALTY,
	/* unsigned int	*/	MPPROV_CONTESTED,
	/* unsigned int	*/	MPPROV_OCCUPREALM,
	/* unsigned int	*/	MPPROV_PILLAGED,

	/* unsigned int	*/	MPPROV_REDUCEHOLDS,
	/* unsigned int	*/	MPPROV_BLESSED,
	/* unsigned int	*/	MPPROV_BLIGHTED,
	/* unsigned int	*/	MPPROV_HONEST,
	/* unsigned int	*/	MPPROV_WARDED,
	/* unsigned int	*/	MPPROV_DISPEL,
	/* unsigned int	*/	MPPROV_DISPELLVL,
	/* unsigned int	*/	MPPROV_LAWCLAIMS,		// 32 bits filled
	/* unsigned int	*/	MPPROV_CASTLESPTD,
	/* unsigned int	*/	MPPROV_HUMANPOP,
	/* unsigned int	*/	MPPROV_ELFPOP,
	/* unsigned int	*/	MPPROV_DWARFPOP,
	/* unsigned int	*/	MPPROV_GOBLINPOP,
	/* unsigned int	*/	MPPROV_LEVYMUSTERED,
	/* unsigned int	*/	MPPROV_TERRAIN,
	/* BORDER_TYPE	*/	MPPROV_BORDER0,
	/* BORDER_TYPE	*/	MPPROV_BORDER1,
	/* BORDER_TYPE	*/	MPPROV_BORDER2,
	/* BORDER_TYPE	*/	MPPROV_BORDER3,
	/* BORDER_TYPE	*/	MPPROV_BORDER4,
	/* BORDER_TYPE	*/	MPPROV_BORDER5,
	/* BORDER_TYPE	*/	MPPROV_BORDER6,
	/* SHORT		*/	MPPROV_FIRSTPLACE,
	/* SHORT		*/	MPPROV_INVESTED,

	MPPROV_MAX_DATA,
} MPPROVINCE_DATA;

typedef enum
{
	/* SHORT		*/	MPPLACE_X,
	/* SHORT		*/	MPPLACE_Y,
	/* unsigned int	*/	MPPLACE_REALM,
	/* unsigned int	*/	MPPLACE_LEVEL,
	/* unsigned int	*/	MPPLACE_ICON,
	/* unsigned int	*/	MPPLACE_IROUTE1,
	/* unsigned int	*/	MPPLACE_IROUTE2,
	/* unsigned int	*/	MPPLACE_IROUTE3,
	/* unsigned int	*/	MPPLACE_BESIEGED,
	/* unsigned int	*/	MPPLACE_CONTESTED,
	/* SHORT        */  MPPLACE_FSITEDISPLAYED,
	/* unsigned int	*/	MPPLACE_INAME,
	/* SHORT		*/	MPPLACE_NEXTPLACE,
	/* CSTRPTR		*/	MPPLACE_SCENE,
	/* SHORT		*/	MPPLACE_IICONBITM,

	MPPLACE_MAX_DATA,
} MPPLACE_DATA;

typedef enum
{
	/* unsigned int	*/	MPUNITS_PROVINCE,
	/* unsigned int	*/	MPUNITS_REALM,
	/* unsigned int	*/	MPUNITS_SALARY,
	/* unsigned int	*/	MPUNITS_ICON,
	/* unsigned int	*/	MPUNITS_MOVING,
	/* unsigned int	*/	MPUNITS_AVOIDMOVE,
	/* unsigned int	*/	MPUNITS_JOINED,
	/* unsigned int	*/	MPUNITS_NOTSPOTTED,
	/* unsigned int	*/	MPUNITS_DISGUISED,
	/* unsigned int	*/	MPUNITS_HASTENED,
	/* unsigned int	*/	MPUNITS_BLESSED,
	/* unsigned int	*/	MPUNITS_PAID,
	/* unsigned int	*/	MPUNITS_USE_REGENCY,
	/* unsigned int	*/	MPUNITS_MOVEPART,
	/* unsigned int	*/	MPUNITS_DESTPROVINCE,
	/* unsigned int	*/	MPUNITS_DESTPATH1,
	/* unsigned int	*/	MPUNITS_DESTPATH2,
	/* unsigned int	*/	MPUNITS_DESTPATH3,
	/* SHORT		*/	MPUNITS_ID,
	/* SHORT		*/	MPUNITS_NEXTUNIT,
	/* SHORT        */	MPUNITS_IICONBITM,
	/* SHORT		*/	MPUNITS_IHANDLE,
	/* UNIT_TYPE	*/	MPUNITS_CREATE,
	MPUNITS_MAX_DATA,
} MPUNITS_DATA;

typedef enum
{
	MPGAMESEMIPHORE_BATTLE_START,
	MPGAMESEMIPHORE_BATTLE_START1,
	MPGAMESEMIPHORE_UNITINDEX,
	MPGAMESEMIPHORE_PLACEINDEX,
	MPGAMESEMIPHORE_DECLAREDWAR,

	MPGAMESEMIPHORE_MAX_DATA,
} MPGAMESEMIPHORE_DATA;

#endif // _MULTIMAP_HXX
