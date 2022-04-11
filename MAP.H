/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _MAP_H
#define _MAP_H


/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_THINGTYP_H)
#include "thingtyp.h"
#endif

#include <limits.h>

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define MAP_ERROR_CHECKED 0


typedef enum {MAP_REG_NONE=0,MAP_REG_NORM=1,MAP_REG_FULL=2} MapRegionType;

typedef struct 
{
	LONG Treasure;
	LONG Enemies;
	LONG SecretDoors;
	LONG Traps;
	LONG Evil;
	LONG InvisibleCreatures;
	LONG Magic;
	LONG QuestObject;

} MapHighlightData;

typedef struct _MAP_INFO {
	POINT			ScreenOrigin;
	POINT 			ScreenClip;
	POINT			ScreenCenter;
	POINT			WorldCenter;
	VECTOR			BoundingBox;
	LONG			ZoomFactor;
	PFV				GameDrawFunc;
	MapRegionType	RegionType;  				//full-screen, normal, none.
	MapHighlightData Highlight;
	

	unsigned long	fDrawMap 			:1;
	unsigned long	fFullScreen 		:1;
	unsigned long	fDrawCheaterLines 	:1;
	unsigned long	fDrawGrid			:1;
	unsigned long	fDrawCamera 		:1;
	unsigned long	fDrawPlayer 		:1;
	unsigned long	fDrawAll 			:1;
	unsigned long	fWaitingForTeleport	:1;
	unsigned long	fColorCycleSSect	:1;
	unsigned long 	fPrintMythingIdxs	:1;
	unsigned long 	fPrintLinedefIdxs	:1;
	unsigned long 	fPrintAvatarHdls	:1;
} MAP_INFO, *PTR_MAP_INFO;

	
	
   
//reserved bits 3-7 (0x0004-0x0040) for map shapes/colors

#define MAP_SHAPE_MASK		(0x0004|0x0008) 
#define MAP_COLOR_MASK		(0x0010|0x0020|0x0040)

#define MS_OCTAGON	0x0000			//these are for attrs
#define MS_TRIANGLE	0x0004
#define MS_DIAMOND	0x0008			
#define MS_DIVROD	(0x0004|0x0008)	
		  
#define MC_DEFAULT	0x0000  		//so are these
#define MC_DONTDRAW	0x0010	//bit 5 set
#define MC_GETTABLE	0x0020	//bit 6 set
#define MC_BUMPABLE	0x0030	//bits 5,6 set
#define MC_PLANT	0x0040	//bit 7 set
#define MC_UNUSED1	0x0050	//bits 7,5 set
#define MC_UNUSED2	0x0060  //bits 7,6 set
#define MC_UNUSED3	0x0070	//bits 7,6,5 set

#define MC_TOOSMALL MC_DONTDRAW

#define MAPCDEF		0x0000	//so are these
#define MAPCNON		0x0010	//bit 5 set			//transparent
#define MAPCPICK	0x0020	//bit 6 set			//gettable
#define MAPCBUMP	0x0030	//bits 5,6 set		//bumpable
#define MAPCPLANT	0x0040	//bit 7 set			//flora
#define MAPCUNUSED1	0x0050	//bits 7,5 set		//nut'n honey
#define MAPCUNUSED2	0x0060	//bits 7,6 set
#define MAPCUNUSED3	0x0070	//bits 7,6,5 set

#define TOGGLE -2

#define TELEPORT_WAIT 1
#define TELEPORT_DONE 2






//map drawing colors
#define MAP_RED			127
#define MAP_BLUE		79
#define MAP_GREEN		199
#define MAP_YELLOW		175
#define MAP_TRANSPARENT	0
#define MAP_WHITE		30


//these probably need to be changed!
#define UTILITY_LINE_COLOR			MAP_YELLOW
#define IMPASSABLE_LINE_COLOR		MAP_WHITE
#define UNCROSSED_SECRET_LINE_COLOR	IMPASSABLE_LINE_COLOR
#define CROSSED_SECRET_LINE_COLOR	MAP_TRANSPARENT
#define CROSSABLE_LINE_COLOR		MAP_BLUE
#define DIF_FLATS_LINE_COLOR		MAP_BLUE
#define HIGHLIGHTED_LINE_COLOR		MAP_RED

#define NO_THING_COLOR			MAP_TRANSPARENT
#define GETTABLE_THING_COLOR	MAP_YELLOW
#define DEFAULT_THING_COLOR		MAP_BLUE
#define BUMPABLE_THING_COLOR	IMPASSABLE_LINE_COLOR
#define PLANT_THING_COLOR		MAP_GREEN
#define UNUSED_COLOR			MAP_TRANSPARENT
#define UNUSED_COLOR			MAP_TRANSPARENT
#define UNUSED_COLOR			MAP_TRANSPARENT

#define HIGHLIGHTED_THING_COLOR		MAP_RED


#define MAP_SHAPE_AUTO			(-1)
#define MAP_COLOR_AUTO			(-1)

#define MAP_ANGLE_AUTO			(291)	//any value that's not in (-256,256)
#define MAP_ANGLED				(292)
#define MAP_NOT_ANGLED			(293)	

#define MAP_SCALED		TRUE
#define MAP_NOT_SCALED	FALSE


/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */


#ifdef __cplusplus
extern "C" 
{
#endif

extern LONG GetMarginReductionCount(void);


//gotta love arbitrary numbers...
#define FULL_LEVEL_ZOOM		666
#define RESTORE_OLD_ZOOM	242
#define SERVICE_ZOOM		69

void MapHandleZoom(LONG);

void SetMapHighlightTreasure(LONG,LONG);
void SetMapHighlightEnemies(LONG,LONG);
void SetMapHighlightSecretDoors(LONG,LONG);
void SetMapHighlightTraps(LONG,LONG);
void SetMapHighlightEvil(LONG,LONG);
void SetMapHighlightInvisibleCreatures(LONG,LONG);
void SetMapHighlightMagic(LONG,LONG);
void SetMapHighlightQuestObject(LONG,LONG);


BOOL SetMapCameraDraw(LONG);
BOOL SetMapPlayerDraw(LONG); 
BOOL SetMapGridDraw(LONG);
BOOL SetMapShowAll(LONG,LONG);
BOOL SetMapDraw(LONG,LONG);
BOOL SetMapFullScreen(LONG,LONG);

PFV SetMapGameDraw(PFV pfGameFunc);
void DrawMap (LONG,LONG,LONG,LONG);
void ZoomMap(LONG,LONG);
void DrawMapStyledObject(POINT* a,LONG angle,ULONG color,LONG shape,LONG radius,BOOL scaled);
void DrawMapThing(LONG,LONG);
void HandleMapAvatar(LONG,SHORT,ULONG,BOOL);

void DrawMapPlayer(void);
void DrawMapCamera(void);
void DrawMapQuestObject(void);


void MapTeleport(LONG code);  

BOOL IsAvatarDead(THINGTYPE);

void ZoomMapAbsolute(LONG,LONG);

BOOL MapIsActive(void);
LONG MapZoomFactor(void);
LONG MapCenterX(void);
LONG MapCenterY(void);
void SetMapCenter(LONG,LONG);

#ifdef __cplusplus
}
#endif
#endif
