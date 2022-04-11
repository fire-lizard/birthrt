/* ========================================================================
   Copyright (c) 1990,1996   Synergistic Software
   All Rights Reserved.
   ========================================================================
   Filename: MAP.C       -Handles the Automap
   Author: Wes Cumberland
   ========================================================================
   Contains the following internal functions:
   DrawMapXLine             -draws a translated line on the map
   DrawMapTrueLine          -draws a line on the map
   MapHandleThings 			-draws things on the map
   DrawMapDiamond           -draws a Diamond on the map
   DrawMapTriangle          -draws a triangle on the map
   DrawMapOctagon           -draws a Octagon on the map
   DrawMapDiviningRod       -draws a Divining Rod on the map
   DrawMapGrid              -draws the grid according to the blockmap
   DrawMapThing             -draws an object on the map with few params
   MapHandleSecretSSect     -handles the visibility of "secret" linedefs
   DrawMapBlockLinedefs     -draws linedefs in a block from the blockmap
   DrawLinedef				-draws a linedef on the map


   Contains the following general functions:
   SetMapDraw               -toggles map mode
   ZoomMap                  -changes the map's zoom factor
   DrawMap                  -shows the map
   DrawMapStyledObject      -draws an object on the map with many params
   DrawMapAvatar            -draws an avatar on the map
   ShowFullMap              -Reveals all the lines and things on the map
   SetMapCameraDraw         -Sets whether to draw the camera on the map
   SetMapPlayerDraw         -Sets whether to draw the player on the map
   SetMapGridDraw           -Sets whether to draw the grid on the map
   SetGameMapDraw           -Sets the function that the higher level uses to
   							-draw the active objects (avatars)


   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"
#include "engine.h"
#include "engint.h"
#include "machine.h"
#include "player.hxx"
#include "thingtyp.h"
#include "map.h"
#include "things.h"
#include "strenum.h"
#include "special.h"
//#include "arrow.h"

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#ifdef INET
#define MAX_ZOOM_FACTOR 20
#else
#define MAX_ZOOM_FACTOR 50
#endif

#define MAP_WAD_BOUNDS_BUFFER 100

#define ZOOMED_BOUNDING_BOX_RAD_X ((gMapInfo.BoundingBox.dx*gMapInfo.ZoomFactor)/2)
#define ZOOMED_BOUNDING_BOX_RAD_Y ((gMapInfo.BoundingBox.dy*gMapInfo.ZoomFactor)/2)



#define MAX_STEP_HEIGHT 25		//duplicate of player.cpp declaration
#define EYE_HEIGHT		4		//duplicate of gamecamr.hxx declaration

#define MAP_COLOR_PLAYER 197	//player's color on map

#define ACTIVE_OBJECT_MAP_SIZE	8
#define OBJECT_MAP_SIZE			4

extern PTR  TTypePtr;
extern LONG G_TTypeSize;
extern LONG frames;
extern ULONG tot_linedefs;
extern ULONG tot_things;
extern LONG	MaxThingLoaded;	
extern TEXTURE textures[];

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */


#define TTATTRIB_FROM_MYTHING(i) (((TTYPE *)(TTypePtr + (G_TTypeSize * mythings[i].type)))->attrib)
#define GET_MAP_THING_COLOR(t)	MapObjectColors[(((TTATTRIB_FROM_MYTHING(t))&MAP_COLOR_MASK)>>4)]
#define GET_MAP_THING_SHAPE(t)	(TTATTRIB_FROM_MYTHING(t)&MAP_SHAPE_MASK)

#define IS_MAGIC(t)	(TTATTRIB_FROM_MYTHING(t)&MAGIC)
#define IS_EVIL(t)	(TTATTRIB_FROM_MYTHING(t)&EVIL)
#define IS_TREASURE(t) (TTATTRIB_FROM_MYTHING(t)&MAPCPICK)
#define IS_PLANT(t) (TTATTRIB_FROM_MYTHING(t)&MAPCPLANT)




#define XY_IN_BNDBOX(X,Y) (\
						  ((X)>=((gMapInfo.WorldCenter.x)-ZOOMED_BOUNDING_BOX_RAD_X))&&\
						  ((X)<=((gMapInfo.WorldCenter.x)+ZOOMED_BOUNDING_BOX_RAD_X))&&\
						  ((Y)>=((gMapInfo.WorldCenter.y)-ZOOMED_BOUNDING_BOX_RAD_Y))&&\
						  ((Y)<=((gMapInfo.WorldCenter.y)+ZOOMED_BOUNDING_BOX_RAD_Y))\
						  )

#define POINT_IN_BNDBOX(p) (XY_IN_BNDBOX((p).x,(p).y))


#define THING_IN_BNDBOX(t) (XY_IN_BNDBOX(mythings[t].x,mythings[t].y))


#define SCREEN_X_TO_WORLD_X(X) ((gMapInfo.WorldCenter.x)+((X-gMapInfo.ScreenCenter.x)*gMapInfo.ZoomFactor))
#define SCREEN_Y_TO_WORLD_Y(Y) ((gMapInfo.WorldCenter.y)+((gMapInfo.ScreenCenter.y-Y)*gMapInfo.ZoomFactor))

#define WORLD_X_TO_SCREEN_X(X) (gMapInfo.ScreenCenter.x+(((X)-(gMapInfo.WorldCenter.x))/gMapInfo.ZoomFactor))
#define WORLD_Y_TO_SCREEN_Y(Y) (gMapInfo.ScreenCenter.y-(((Y)-(gMapInfo.WorldCenter.y))/gMapInfo.ZoomFactor))


/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
static void DrawMapTrueLine(POINT* a, POINT* b, ULONG color);
static void DrawMapXLine(POINT a,POINT b,ULONG color);
static void DrawMapTriangle( POINT *c,LONG angle,LONG radius,ULONG color);
static void DrawMapDiamond( POINT *center,LONG angle, LONG r,ULONG color);
static void DrawMapOctagon( POINT *c,LONG angle,LONG radius,ULONG color);
static void DrawMapDiviningRod( POINT *c,LONG angle, LONG radius,ULONG color);
static void MapHandleThings(void);
static void DrawMapGrid(void);
static void MapHandleSecretSSect(void);
static void DrawMapLinedef(long i);
static void DrawMapBlockLinedefs(SHORT * bm);


/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */

static UBYTE MapObjectColors[8]={
								 DEFAULT_THING_COLOR,
								 NO_THING_COLOR,
								 GETTABLE_THING_COLOR,
								 BUMPABLE_THING_COLOR,
								 PLANT_THING_COLOR,
								 UNUSED_COLOR,
								 UNUSED_COLOR,
								 UNUSED_COLOR,
								};

static MAP_INFO	gMapInfo = {
						{0, 0},			// ScreenOrigin
						{0, 0},			// ScreenClip
						{0, 0},			// ScreenCenter
						{0, 0},			// WorldCenter.x, WorldCenter.y,
						{0, 0},			// BoundingBox Vector
						5,				// Zoom factor
						NULL,			// Game Draw Function
						MAP_REG_NONE,	// region type
						{0,0,0,0,0,0,0,0}, // Highlight data
						FALSE,			// draw map
						FALSE,			// full screen
						FALSE,			// draw cheater lines
						FALSE,			// draw grid
						TRUE,			// draw camera
						TRUE,			// draw player
						FALSE,			// draw all
						FALSE,			// waiting for teleport
#ifdef _DEBUG
						TRUE,			// color cycle current ssector
#else
						FALSE,			// color cycle current ssector
#endif
						FALSE,			// print mythings numbers
						FALSE,			// print linedefs numbers
						FALSE,			// print avatar handles
						
						};






/* ========================================================================
   Function    - MapRegionFunction
   Description - This is the function that is hooked to the Map region
   Returns     - void
   ======================================================================== */
static void MapRegionFunction(LONG a,LONG b)
{
	LONG WorldX,WorldY;
	b=a;
	
//	printf("in map region func\n");


	if (gMapInfo.fWaitingForTeleport)
	{
//		printf("innit again\n");
		WorldX=SCREEN_X_TO_WORLD_X(cursor_x);
		WorldY=SCREEN_Y_TO_WORLD_Y(cursor_y);
//		printf("Map Clicked at coords (%li,%li)\n",cursor_x,cursor_y);
//		printf("camera (%li,%li) mapcntr(%li,%li)\n",(gMapInfo.WorldCenter.x),(gMapInfo.WorldCenter.y),gMapInfo.ScreenCenter.x,gMapInfo.ScreenCenter.y);
//		printf("transformed to world coords(%li,%li)\n",WorldX,WorldY);
//		printf("you clicked in sector %li ssector %li\n",point_to_sector(WorldX,WorldY),find_ssector(WorldX,WorldY));
//		printf("you teleported a distance of %li\n",aprox_dist((gMapInfo.WorldCenter.x),(gMapInfo.WorldCenter.y),WorldX,WorldY));
//		printf("\n");

		SetPlayer(WorldX,WorldY,point_to_floor_height(WorldX,WorldY),camera.a,camera.p);
		
		SetCameraPosition(
			&camera,
			PLAYER_INT_VAL(player.x),
			PLAYER_INT_VAL(player.y),
			(player.z+(player.h - EYE_HEIGHT)), 
			player.a, 
			player.p
			);
		
		MapTeleport(TELEPORT_DONE);
	}
}

/* ========================================================================
   Function    - MapTeleport
   Description - waits for a click and teleports to where the user clicked
   Returns     - void
   ======================================================================== */
void MapTeleport(LONG code)
{
	static LONG PrevFS=0;
	static LONG PrevDM=0;

	static POINT PrevOrigin={0,0};
	static POINT PrevCenter={0,0};

	LONG r,l,t,b;
	get_margin_size(&r,&l,&t,&b);

	if (code==TELEPORT_WAIT)
	{
//		printf("waiting\n");
		fPause=TRUE;
		PrevFS=gMapInfo.fFullScreen;
		gMapInfo.fFullScreen=TRUE;
		PrevDM=gMapInfo.fDrawMap;
		gMapInfo.fDrawMap=TRUE;
	

		PrevOrigin=gMapInfo.ScreenOrigin;
		PrevCenter=gMapInfo.ScreenCenter;

		gMapInfo.fFullScreen=TRUE;

		MapHandleZoom(FULL_LEVEL_ZOOM);
	
		fPause=TRUE;
		
		// TODO: (fire lizard) uncomment
		//HdlMapRegion();
		gMapInfo.fWaitingForTeleport=TRUE;
	}

	else if (code==TELEPORT_DONE)
	{
//		printf("done\n");
		fPause=FALSE;
		
		gMapInfo.fDrawMap=PrevFS;
		gMapInfo.fFullScreen=FALSE;

		MapHandleZoom(RESTORE_OLD_ZOOM);

		fPause=FALSE;
		// TODO: (fire lizard) uncomment
		//HdlMapRegion();
		gMapInfo.fWaitingForTeleport=FALSE;
	}
	
}


	
	
	




static void AddMapRegion(void)
{
//	printf("added map reg func (%li,%li) (%lix%li)  (%li,%li)\n",gMapInfo.ScreenOrigin.x,gMapInfo.ScreenOrigin.y,gMapInfo.ScreenClip.x-gMapInfo.ScreenOrigin.x,gMapInfo.ScreenClip.x-gMapInfo.ScreenOrigin.x,gMapInfo.ScreenClip.x,gMapInfo.ScreenClip.y);
	add_region(gMapInfo.ScreenOrigin.x,gMapInfo.ScreenOrigin.y,gMapInfo.ScreenClip.x-gMapInfo.ScreenOrigin.x,gMapInfo.ScreenClip.y-gMapInfo.ScreenOrigin.y,
			NO_KEY,MapRegionFunction,0,0,69,-1);
}


static void DelMapRegion(void)
{
//	printf("deled map reg func\n");
	del_region(MapRegionFunction,NO_KEY);
}

static void HdlMapRegion(void)
{
	static LONG LastFrameMapRegionType=MAP_REG_NONE;  //wow, long name

	static MAP_INFO OldMapInfo={{0,0},{0,0},{0,0},{0,0}}; //we only worry about the coordinate fields...


	//this algorithm is nonsensical, but it's compact and it works.
	// chart:  DrawMap FullScrn RegionType
	//				1 	  1 		= 2
	//				0 	  1 		= 1
	//				0 	  0 		= 0
	//				1 	  0  		= 0
	gMapInfo.RegionType=(MapRegionType)((gMapInfo.fFullScreen&&gMapInfo.fDrawMap)+gMapInfo.fDrawMap);

	// if the region type has 	
	if (LastFrameMapRegionType!=gMapInfo.RegionType || 
		OldMapInfo.ScreenCenter.y!=gMapInfo.ScreenCenter.y)
	{
		if (LastFrameMapRegionType!=MAP_REG_NONE)
			DelMapRegion();
		if (gMapInfo.RegionType!=MAP_REG_NONE)
			AddMapRegion();
	}

	//assign statics
	LastFrameMapRegionType=gMapInfo.RegionType;
	OldMapInfo=gMapInfo;

	
}


/* ========================================================================
   Function    - SetMapHighlightEnemies
   Description - Sets the range to highlight other avatars
   Returns     - void
   ======================================================================== */
void SetMapHighlightEnemies(LONG unused,LONG arg)
{
	//since this is not a flag, we can't "toggle" it, so this equation
	//will suffice. this equation only works if !0 == 1.

//	printf("highlighting\n");
	gMapInfo.Highlight.Enemies=arg*(!gMapInfo.Highlight.Enemies);
	
//	printf("MHE=%li\n",gMapInfo.Highlight.Enemies);
}

/* ========================================================================
   Function    - SetMapHighlightTreasure
   Description - Sets the range to highlight Treasure
   Returns     - void
   ======================================================================== */
void SetMapHighlightTreasure(LONG unused,LONG arg)
{
	//since this is not a flag, we can't "toggle" it, so this equation
	//will suffice. this equation only works if !0 == 1.

	//printf("highlighting\n");
	gMapInfo.Highlight.Treasure=arg*(!gMapInfo.Highlight.Treasure);
}

/* ========================================================================
   Function    - SetMapHighlightEvil
   Description - Sets the range to highlight Evil
   Returns     - void
   ======================================================================== */
void SetMapHighlightEvil(LONG unused,LONG arg)
{
	//since this is not a flag, we can't "toggle" it, so this equation
	//will suffice. this equation only works if !0 == 1.

	//printf("highlighting\n");
	gMapInfo.Highlight.Evil=arg*(!gMapInfo.Highlight.Evil);
}

/* ========================================================================
   Function    - SetMapHighlightSecretDoors
   Description - Sets the range to highlight SecretDoors
   Returns     - void
   ======================================================================== */
void SetMapHighlightSecretDoors(LONG unused,LONG arg)
{
	//since this is not a flag, we can't "toggle" it, so this equation
	//will suffice. this equation only works if !0 == 1.

	//printf("highlighting\n");
	gMapInfo.Highlight.SecretDoors=arg*(!gMapInfo.Highlight.SecretDoors);
}

/* ========================================================================
   Function    - SetMapHighlightTraps
   Description - Sets the range to highlight Traps
   Returns     - void
   ======================================================================== */
void SetMapHighlightTraps(LONG unused,LONG arg)
{
	//since this is not a flag, we can't "toggle" it, so this equation
	//will suffice. this equation only works if !0 == 1.

	//printf("highlighting\n");
	gMapInfo.Highlight.Traps=arg*(!gMapInfo.Highlight.Traps);
}

/* ========================================================================
   Function    - SetMapHighlightMagic
   Description - Sets the range to highlight Magic
   Returns     - void
   ======================================================================== */
void SetMapHighlightMagic(LONG unused,LONG arg)
{
	//since this is not a flag, we can't "toggle" it, so this equation
	//will suffice. this equation only works if !0 == 1.
	//printf("highlighting\n");

	gMapInfo.Highlight.Magic=arg*(!gMapInfo.Highlight.Magic);
}

/* ========================================================================
   Function    - SetMapHighlightInvisibleCreatures
   Description - Sets the range to highlight InvisibleCreatures
   Returns     - void
   ======================================================================== */
void SetMapHighlightInvisibleCreatures(LONG unused,LONG arg)
{
	//since this is not a flag, we can't "toggle" it, so this equation
	//will suffice. this equation only works if !0 == 1.

	//printf("highlighting\n");
	gMapInfo.Highlight.InvisibleCreatures=arg*(!gMapInfo.Highlight.InvisibleCreatures);
}

/* ========================================================================
   Function    - SetMapHighlightInvisibleCreatures
   Description - Flags the map to draw the Quest Object 
   Returns     - void
   ======================================================================== */
void SetMapHighlightQuestObject(LONG unused,LONG arg)
{
	if (arg==TOGGLE)
		gMapInfo.Highlight.QuestObject=!gMapInfo.Highlight.QuestObject;
	else
		gMapInfo.Highlight.QuestObject=arg;
}

/* ========================================================================
   Function    - SetMapCameraDraw
   Description - Sets whether or not to draw the Camera on the map
   Returns     - What it was set to before
   ======================================================================== */
BOOL SetMapCameraDraw(LONG arg)
{
	BOOL og=gMapInfo.fDrawCamera;
	if (arg==TOGGLE)
		gMapInfo.fDrawCamera=!gMapInfo.fDrawCamera;
	else
		gMapInfo.fDrawCamera=arg;
	return og;
}

/* ========================================================================
   Function    - SetMapPlayerDraw
   Description - Sets whether or not to draw the Player on the map
   Returns     - What it was set to before
   ======================================================================== */
BOOL SetMapPlayerDraw(LONG arg)
{
	BOOL og=gMapInfo.fDrawPlayer;
	if (arg==TOGGLE)
		gMapInfo.fDrawPlayer=!gMapInfo.fDrawPlayer;
	else
		gMapInfo.fDrawPlayer=arg;
	return og;
}

/* ========================================================================
   Function    - SetMapGridDraw
   Description - Sets whether or not to draw the Grid on the Map
   Returns     - What it was set to before
   ======================================================================== */
BOOL SetMapGridDraw(LONG arg)
{
	BOOL og=gMapInfo.fDrawGrid;

	if (arg==TOGGLE)
		gMapInfo.fDrawGrid=!gMapInfo.fDrawGrid;
	else
		gMapInfo.fDrawGrid=arg;
	return og;
}

/* =======================================================================
   Function    - SetGameMapDraw
   Description - setup pointer to function for game map draw (avatars now)
   Returns     - the old drawing function
   ======================================================================== */
PFV SetMapGameDraw(PFV pfGameFunc)
{
	PFV ReturnMe=gMapInfo.GameDrawFunc;
	gMapInfo.GameDrawFunc = pfGameFunc;
	return ReturnMe;
}


/* ========================================================================
   Function    - SetMapFullScreen
   Description - toggles FS map mode
   Returns     - the old value
   ======================================================================== */
BOOL SetMapFullScreen(LONG unused, LONG arg)
{
	BOOL old=gMapInfo.fFullScreen;

	if (arg==TOGGLE)
	{
		gMapInfo.fFullScreen=!gMapInfo.fFullScreen;
		
	}
	else
	{
		gMapInfo.fFullScreen = arg;
	}

	return old;
}



/* ========================================================================
   Function    - SetMapDraw
   Description - toggles map mode
   Returns     - void
   ======================================================================== */
BOOL SetMapDraw(LONG unused, LONG arg)
{
	
	BOOL old=gMapInfo.fDrawMap;

	if (arg==TOGGLE)
	{
		if (gMapInfo.fDrawMap && !gMapInfo.fFullScreen)
		{
			gMapInfo.fFullScreen=TRUE;

			MapHandleZoom(FULL_LEVEL_ZOOM);
	
			fPause=TRUE;
		}
		else if (gMapInfo.fDrawMap && gMapInfo.fFullScreen)
		{
			gMapInfo.fDrawMap=FALSE;
			gMapInfo.fFullScreen=FALSE;

			MapHandleZoom(RESTORE_OLD_ZOOM);

			fPause=FALSE;
			HdlMapRegion();
		}
		else
			gMapInfo.fDrawMap=TRUE;
		
	}
	else
	{
		gMapInfo.fDrawMap = arg;	
	}
	
	return old;
}

/* ========================================================================
   Function    - SetMapShowAll
   Description - Shows the whole map
   Returns     - void
   ======================================================================== */
BOOL SetMapShowAll(LONG unused1,LONG arg)
{
	BOOL old=gMapInfo.fDrawAll;

	//printf("sm%li\n",arg);

	if (arg==TOGGLE)
		gMapInfo.fDrawAll=!gMapInfo.fDrawAll;
	else
		gMapInfo.fDrawAll=arg;
		
	return old;
}	



/* =======================================================================
   Function    - DrawMap
   Description - shows the map
   Returns     - void
   ======================================================================== */
void DrawMap (LONG xx, LONG yy, LONG ww, LONG hh)
{
	LONG		u;
	LONG 		NegXExt;
	LONG 		PosXExt;
	LONG 		NegYExt;
	LONG 		PosYExt;

	POINT		OldScreenOrigin;
	POINT		OldClip;

	LONG r,l,t,b;
	get_margin_size(&r,&l,&t,&b);

	if (gMapInfo.fFullScreen)
	{
		xx=l;
		yy=t;
		ww=MAX_VIEW_WIDTH-r-l;
		hh=MAX_VIEW_HEIGHT-b-t;
		MapHandleZoom(SERVICE_ZOOM);
		fPause=TRUE;
		
	}
	else
	{
		gMapInfo.WorldCenter.x=CAMERA_INT_VAL(camera.x);
		gMapInfo.WorldCenter.y=CAMERA_INT_VAL(camera.y);
	}

	
	//save the old data, as not to screw up the other stuff that relies on 
	//ScreenOrigin.x and xClip
	OldScreenOrigin.x=origin_x;
	OldScreenOrigin.y=origin_y;
	OldClip.x=clip_x;
	OldClip.y=clip_y;
	
	
	
	
	
	// Save this data for other routines which need information about the map.
	gMapInfo.ScreenOrigin.x= origin_x=xx;
	gMapInfo.ScreenOrigin.y= origin_y=yy;
	gMapInfo.ScreenClip.x  = clip_x  =(xx+ww);
	gMapInfo.ScreenClip.y  = clip_y  =(yy+hh);
	gMapInfo.ScreenCenter.x=(xx+(ww/2));
	gMapInfo.ScreenCenter.y=(yy+(hh/2));
	

	gMapInfo.BoundingBox.dx=ww;
	gMapInfo.BoundingBox.dy=hh;

	shade_edged_rect(xx,yy,ww,hh,33);
	
	



	NegXExt=(-(ZOOMED_BOUNDING_BOX_RAD_X/128))-1;
	PosXExt=(ZOOMED_BOUNDING_BOX_RAD_X/128)+1;
	NegYExt=(-(ZOOMED_BOUNDING_BOX_RAD_Y/128))-1;
	PosYExt=(ZOOMED_BOUNDING_BOX_RAD_Y/128)+1;


	for (u=NegXExt;u<=PosXExt;++u)
	{
		LONG		v;
		LONG const ux = gMapInfo.WorldCenter.x+u*128;
		
		for (v=NegYExt;v<=PosYExt;++v)
		{
			SHORT * const bm = get_blockm (ux,(gMapInfo.WorldCenter.y)+v*128);
			if(bm)
				DrawMapBlockLinedefs(bm);
		}
	}

	
	HdlMapRegion();
	MapHandleThings();
	MapHandleSecretSSect();

	if (gMapInfo.fDrawGrid)				DrawMapGrid();
	// TODO: (fire lizard) uncomment
	//if (gMapInfo.fDrawCheaterLines) 	DrawMapCheaterArrows();
	if (gMapInfo.GameDrawFunc) 			(*gMapInfo.GameDrawFunc)();	//execute the game-level's callback
	if (gMapInfo.fDrawCamera) 			DrawMapCamera();
	if (gMapInfo.fDrawPlayer) 			DrawMapPlayer();
	if (gMapInfo.Highlight.QuestObject)		DrawMapQuestObject();
	

	


		
	origin_x=OldScreenOrigin.x;
	origin_y=OldScreenOrigin.y;
	clip_x=OldClip.x;
	clip_y=OldClip.y;
}


void DrawMapPlayer(void)
{
	POINT p;
	LONG PrintX,PrintY;
	p.x=PLAYER_INT_VAL(player.x);
	p.y=PLAYER_INT_VAL(player.y);

	PrintX=WORLD_X_TO_SCREEN_X(p.x)-70/gMapInfo.ZoomFactor;
	PrintY=WORLD_Y_TO_SCREEN_Y(p.y)-100/gMapInfo.ZoomFactor;
	
	
	if (gMapInfo.fFullScreen)
	{
		
		DrawMapStyledObject(&p,player.a,MAP_RED,MS_DIVROD,
							30,MAP_SCALED);

		print_textf(PrintX,PrintY,MAP_YELLOW,STRMGR_GetStr(STR_PARTY));
		
	}
	else
		DrawMapStyledObject(&p,player.a,MAP_COLOR_PLAYER,MS_DIVROD,
							player.w/2,MAP_SCALED);
	
}


void DrawMapCamera(void)
{
	POINT p;
	//Draw the camera (temporary?)
	p.x=CAMERA_INT_VAL(camera.x);
	p.y=CAMERA_INT_VAL(camera.y);

	if (gMapInfo.fFullScreen)
		DrawMapStyledObject(&p,camera.a,
							MAP_YELLOW,
							MS_TRIANGLE,20,MAP_SCALED);
	else
		DrawMapStyledObject(&p,camera.a,
							MAP_BLUE,
							MS_TRIANGLE,8,MAP_SCALED);
}



LONG GetLevelFiendIndex(void);
void DrawMapQuestObject(void)
{
	LONG i;
	static LONG QuestObjIndex=-1;
	static BOOL	fQobjInFiend=FALSE;


	//check to see if the Index is still valid, if not, scan the wad
	//to get the new one.

	if (mythings[QuestObjIndex].type!=GetQuestThing() && !fQobjInFiend)
	{
		for (i=0;i<MAX_THINGS;++i)
			if (mythings[i].valid)
				if (mythings[i].type==GetQuestThing())
	 			{
					QuestObjIndex=i;
					break;
				}
	}

	//if it's not in the wad, scan for a fiend, he or she (gotta be PC) 
	//probably has it.  An additional scan of inventory may be in order
	//to determine if he/she actually has the item.
	if (i==MAX_THINGS)
	{
		LONG FiendIndex=GetLevelFiendIndex();
		if (FiendIndex==-1)
			return;

		QuestObjIndex=FiendIndex;
		fQobjInFiend=TRUE;
	}
	
	
			
	if (QuestObjIndex!=-1)
	{			
		
		POINT a;
		LONG PrintX,PrintY;
		
		MYTHING mt=mythings[QuestObjIndex];
		
		a.x=mt.x;
		a.y=mt.y;
		printf("QI at %li,%li\n",mt.x,mt.y);
		DrawMapStyledObject(&a,MAP_NOT_ANGLED,MAP_RED,MS_DIAMOND,30,MAP_SCALED);
		DrawMapStyledObject(&a,MAP_NOT_ANGLED,MAP_RED,MS_DIAMOND,15,MAP_SCALED);
		
		PrintX=WORLD_X_TO_SCREEN_X(a.x)-100/gMapInfo.ZoomFactor;
		PrintY=WORLD_Y_TO_SCREEN_Y(a.y)-100/gMapInfo.ZoomFactor;

		if (!fQobjInFiend)
			print_textf(PrintX,PrintY,MAP_YELLOW,STRMGR_GetStr(STR_MAP_QUEST_OBJECT));
		else
			print_textf(PrintX,PrintY,MAP_YELLOW,STRMGR_GetStr(STR_MAP_QUEST_OBJECT_FIEND));
			
	}
	
}


	


/* ========================================================================
   Function    - DrawMapStyledObject
   Description - draws a map object w/ lots of params
   Returns     - void
   ======================================================================== */
void DrawMapStyledObject(POINT* a,LONG angle,ULONG color,LONG shape,
						 LONG radius,BOOL scaled)
{
	LONG	oldMapFactor;

	
	if (!scaled)
	{
		oldMapFactor = gMapInfo.ZoomFactor;
		if (fHighRes)
			gMapInfo.ZoomFactor = 2;
		else
			gMapInfo.ZoomFactor = 4;
	}
				
	switch(shape)
	{
		case MS_DIAMOND:
			DrawMapDiamond(a,angle,radius,color);
			break;
		case MS_TRIANGLE:
			DrawMapTriangle(a,angle,radius,color);
			break;
		case MS_OCTAGON:
			DrawMapOctagon(a,angle,radius,color);
			break;
		case MS_DIVROD:
			DrawMapDiviningRod(a,angle,radius,color);
			break;
	}

	if (!scaled)
		gMapInfo.ZoomFactor = oldMapFactor;


}


/* ========================================================================
   Function    - HandleMapAvatar
   Description - Determines if the specified avatar can be drawn, and if so
                 draws it...   The argument hAvatar is for debugging 
                 purposes only...
   Returns     - void
   ======================================================================== */
void HandleMapAvatar(LONG i,SHORT hAvatar,ULONG color,BOOL scaled)
{
	LONG shape,angle,radius;
	POINT o;


	if (mythings[i].dist>>8 > gMapInfo.Highlight.Enemies  &&  !mythings[i].fDrawn)
		return;

	
	if (THING_IN_BNDBOX(i))
	{
		shape=GET_MAP_THING_SHAPE(i);
		
		angle=mythings[i].angle;
		
		o.x=mythings[i].x;
		o.y=mythings[i].y;
		
		radius=mythings[i].widScaled/4;
		
		DrawMapStyledObject(&o,angle,color,shape,radius,scaled);


		if (gMapInfo.fPrintMythingIdxs || gMapInfo.fPrintAvatarHdls)
		{
			LONG PrintX,PrintY;
			char buffer[15]="";
			PrintX=WORLD_X_TO_SCREEN_X(o.x-20);
			PrintY=WORLD_Y_TO_SCREEN_Y(o.y+10);

			if (gMapInfo.fPrintMythingIdxs)	
				sprintf(buffer,"[%li]",i);
			if (gMapInfo.fPrintAvatarHdls)
				sprintf(buffer,"%s(%li)",buffer,hAvatar);
							
			print_textf(PrintX,PrintY,MAP_YELLOW,"[%li]",i);
		}
	}


}
	

/* ========================================================================
   Function    - DrawMapThing
   Description - Draws an object on the map
   Returns     - void
   ======================================================================== */
void DrawMapThing(LONG i,LONG angled)
{
	LONG color,shape,angle,radius;
	POINT o;

	color=GET_MAP_THING_COLOR(i);
	shape=GET_MAP_THING_SHAPE(i);

	if (angled==MAP_ANGLED)			//This is a Kludge
		angle=mythings[i].angle;
	else
		angle=MAP_NOT_ANGLED;

	o.x=mythings[i].x;
	o.y=mythings[i].y;

	if (IsHalfWidthBumpable)
	{
		radius=mythings[i].widScaled/4;
	}
	else
	{
		radius=mythings[i].widScaled/2;
	}

	
	if (color!=MAP_TRANSPARENT)
	{

		DrawMapStyledObject(&o,angle,color,shape,radius,MAP_SCALED);
	}
}

/* ========================================================================
   Function    - DrawMapBoundingBox
   Description - Draws the bounding box that the map uses to determine which
                 things to draw.
   Returns     - void
   ======================================================================== */
void DrawMapBoundingBox(void)
{
#if _DEBUG
	
	POINT w,x,y,z;


	//(should never get drawn, but just in case)
	//draw bound box
	w.x=(gMapInfo.WorldCenter.x)-ZOOMED_BOUNDING_BOX_RAD_X;	//3rd Quad
	w.y=(gMapInfo.WorldCenter.y)-ZOOMED_BOUNDING_BOX_RAD_Y;
	x.x=(gMapInfo.WorldCenter.x)+ZOOMED_BOUNDING_BOX_RAD_X; //1st Quad
	x.y=(gMapInfo.WorldCenter.y)+ZOOMED_BOUNDING_BOX_RAD_Y;
	y.x=(gMapInfo.WorldCenter.x)-ZOOMED_BOUNDING_BOX_RAD_X; //2nd Quad
	y.y=(gMapInfo.WorldCenter.y)+ZOOMED_BOUNDING_BOX_RAD_Y;
	z.x=(gMapInfo.WorldCenter.x)+ZOOMED_BOUNDING_BOX_RAD_X; //4th Quad
	z.y=(gMapInfo.WorldCenter.y)-ZOOMED_BOUNDING_BOX_RAD_Y;


	DrawMapXLine(x,y,MAP_YELLOW);
	DrawMapXLine(y,w,MAP_YELLOW);
	DrawMapXLine(w,z,MAP_YELLOW);
	DrawMapXLine(z,x,MAP_YELLOW);
#endif
}

/* ========================================================================
   Function    - DrawMapCheaterArrows
   Description - Draws lines from the player to the exit line
   Returns     - void
   ======================================================================== */
void DrawMapCheaterArrows(void)
{
	LONG ExitLines[100];
	LONG CurrExitLine=0;
	LONG i=0;
	POINT Exitpa,Plr;

//	printf("here\n");

	while (i<tot_linedefs)
	{
		if (linedefs[i].special==11 || linedefs[i].special==52)
			ExitLines[CurrExitLine++]=i;
		++i;
	}
	i=0;
	while (i<CurrExitLine)
	{
		Exitpa.x=vertexs[linedefs[ExitLines[i]].a].x;
		Exitpa.y=vertexs[linedefs[ExitLines[i]].a].y;
		Plr.x=(gMapInfo.WorldCenter.x);
		Plr.y=(gMapInfo.WorldCenter.y);

		DrawMapXLine(Exitpa,Plr,MAP_BLUE);
		++i;
	}
}





/* ========================================================================
   Function    - DrawMapLinedef
   Description - Determines which color to draw and draws a linedef
   Returns     - void
   ======================================================================== */
static void DrawMapLinedef(long i)
{
	POINT a,b;
	int iSectorF,iSectorB;
	LONG PrintX; 
	LONG PrintY; 

	LONG btex,ttex;
	

	LINEDEF * const Ld=&linedefs[i];
//	SECTOR FrontSect,BackSect;

	if ((Ld->flags&DRAW_ON_MAP_BIT && !(Ld->flags&NOT_ON_MAP_BIT) )
		||gMapInfo.fDrawAll)
	
	{
				
		a.x=vertexs[Ld->a].x;
		a.y=vertexs[Ld->a].y;
		b.x=vertexs[Ld->b].x;
		b.y=vertexs[Ld->b].y;
		
		PrintX=WORLD_X_TO_SCREEN_X((a.x+b.x)/2); 
		PrintY=WORLD_Y_TO_SCREEN_Y((a.y+b.y)/2); 
		
					
		if (Ld->psdt < 0)
		{
			DrawMapXLine(a,b,IMPASSABLE_LINE_COLOR); //onesided!
			return;
		}

		
		if (gMapInfo.Highlight.Traps)
			if (Ld->special || Ld->tag)
			{
				DrawMapXLine(a,b,HIGHLIGHTED_LINE_COLOR);
				return;
			}

	   	iSectorF = sidedefs[Ld->psdb].sec_ptr;

	   	iSectorB = sidedefs[Ld->psdt].sec_ptr;
		
//for readability.  
#define MIDDLE_TEX n3
#define UPPER_TEX  n1
#define LOWER_TEX  n2
		
		btex=sidedefs[Ld->psdb].MIDDLE_TEX;
		ttex=sidedefs[Ld->psdt].MIDDLE_TEX;

		if (gMapInfo.Highlight.SecretDoors)
			if (Ld->flags&TWO_SIDED_BIT)
				if (!(Ld->flags&IMPASSABLE_BIT))
				{

 					if ((btex && textures[btex].type!=TRANSP_TEX) || (ttex && textures[ttex].type!=TRANSP_TEX)) 
					{
						//this is sooooooooooooooooooo kludgy.
						if (btex)
							if (textures[btex].name[0]=='D' &&
								textures[btex].name[1]=='O' &&
								textures[btex].name[2]=='O' &&
								textures[btex].name[3]=='R')
								return;

						if (ttex)
							if (textures[ttex].name[0]=='D' &&
								textures[ttex].name[1]=='O' &&
								textures[ttex].name[2]=='O' &&
								textures[ttex].name[3]=='R')
								return;
		
						DrawMapXLine(a,b,HIGHLIGHTED_LINE_COLOR);
						return;
					}
				}
					



		if (Ld->flags & IMPASSABLE_BIT)
			DrawMapXLine(a,b,IMPASSABLE_LINE_COLOR);

				
		else if (Ld->flags & SECRET_BIT)
		{
			if (gMapInfo.Highlight.SecretDoors)
				DrawMapXLine(a,b,HIGHLIGHTED_LINE_COLOR);

			else if (!(Ld->flags & HAS_BEEN_CROSSED))
				DrawMapXLine(a,b,UNCROSSED_SECRET_LINE_COLOR);
			else
				DrawMapXLine(a,b,CROSSED_SECRET_LINE_COLOR);
		}
		
		else if (sectors[iSectorF].fh != sectors[iSectorB].fh)
			DrawMapXLine(a,b,CROSSABLE_LINE_COLOR);

		else if (Ld->flags & TWO_SIDED_BIT)
			if (btex || ttex)
				DrawMapXLine(a,b,IMPASSABLE_LINE_COLOR);

		if (gMapInfo.fPrintLinedefIdxs)
			print_textf(PrintX,PrintY,MAP_YELLOW,"[%li]",i);
		
	}
}	   
/* ========================================================================
   Function    - DrawMapBlockLinedefs
   Description - Draws the Linedefs in a specified block from the blockmap
   Returns     - void
   ======================================================================== */
static void DrawMapBlockLinedefs(SHORT * bm)
{
	LONG i=0;
	
	while(bm[i]!=-1 && bm[i]<tot_linedefs)
		DrawMapLinedef(bm[i++]);
}		


/* =======================================================================
   Function    - DrawMapXLine
   Description - draws a line on the map
   Returns     - void
   ======================================================================== */
static void DrawMapXLine(POINT a,POINT b,ULONG color)
{
	POINT gmiWCenter;

	gmiWCenter.x=gMapInfo.WorldCenter.x;
	gmiWCenter.y=gMapInfo.WorldCenter.y;

	
	//xlatePoint(&a,gmiWCenter);
	a.x = a.x - gmiWCenter.x;
	a.y = a.y - gmiWCenter.y;
	//xlatePoint(&b,gmiWCenter);
	b.x = b.x - gmiWCenter.x;
	b.y = b.y - gmiWCenter.y;

	a.y = -(a.y);
	b.y = -(b.y);

	DrawMapTrueLine( &a, &b, color );
}

/* ========================================================================
   Function    - DrawMapTrueLine
   Description - draws a line on the map
   Returns     -
   ======================================================================== */
static void DrawMapTrueLine( POINT* a, POINT* b, ULONG color)
{
	LONG ax;
	LONG bx;
	LONG ay;
	LONG by;
	
	if (gMapInfo.ZoomFactor==0)
		gMapInfo.ZoomFactor=2;		
	
	ax = (a->x/gMapInfo.ZoomFactor)+gMapInfo.ScreenCenter.x;
	ay = (a->y/gMapInfo.ZoomFactor)+gMapInfo.ScreenCenter.y;
	bx = (b->x/gMapInfo.ZoomFactor)+gMapInfo.ScreenCenter.x;
	by = (b->y/gMapInfo.ZoomFactor)+gMapInfo.ScreenCenter.y;

	line (ax, ay, bx, by, color);
	if (fHighRes)
	{
		LONG	deltaX, deltaY;
		
		// GWP line((a->x/gMapInfo.ZoomFactor)+gMapInfo.ScreenCenter.x,((a->y/gMapInfo.ZoomFactor))+gMapInfo.ScreenCenter.y,
		//	(b->x/gMapInfo.ZoomFactor)+gMapInfo.ScreenCenter.x,((b->y/gMapInfo.ZoomFactor))+gMapInfo.ScreenCenter.y,color);
		
		deltaX = a->x - b->x;
		deltaY = a->y - b->y;
		
		if(abs(deltaX) > abs(deltaY))
		{
			//line((a->x/gMapInfo.ZoomFactor)+gMapInfo.ScreenCenter.x,((a->y/gMapInfo.ZoomFactor))+gMapInfo.ScreenCenter.y+1,
			//	(b->x/gMapInfo.ZoomFactor)+gMapInfo.ScreenCenter.x,((b->y/gMapInfo.ZoomFactor))+gMapInfo.ScreenCenter.y+1,color);
			line (ax, ay + 1, bx, by + 1, color);
		}
		else
		{
			//line((a->x/gMapInfo.ZoomFactor)+gMapInfo.ScreenCenter.x+1,((a->y/gMapInfo.ZoomFactor))+gMapInfo.ScreenCenter.y,
			//	(b->x/gMapInfo.ZoomFactor)+gMapInfo.ScreenCenter.x+1,((b->y/gMapInfo.ZoomFactor))+gMapInfo.ScreenCenter.y,color);
			line (ax+1, ay, bx+1, by, color);
		}
	}
	// GWP else
	// GWP {
	// GWP 	line((a->x/gMapInfo.ZoomFactor)+gMapInfo.ScreenCenter.x,((a->y/gMapInfo.ZoomFactor))+gMapInfo.ScreenCenter.y,
	// GWP 		(b->x/gMapInfo.ZoomFactor)+gMapInfo.ScreenCenter.x,((b->y/gMapInfo.ZoomFactor))+gMapInfo.ScreenCenter.y,color);
	// GWP }
}




/* ========================================================================
   Function    - DrawMapDiamond
   Description - draws a diamond around a given center with a given radius
   Returns     - nut'n honey
   ======================================================================== */
static void DrawMapDiamond( POINT *center,LONG angle, LONG r,ULONG color)
{
	POINT a,b,c,d;				//    a
								//   d b
								//    c
	//create diamond
	a.x=0;
	a.y=+r;
	b.x=r;
	b.y=0;		
	c.x=0;
	c.y=-r;
	d.x=-r;
	d.y=0;


	//rotate to the object's angle
	if (angle!=MAP_NOT_ANGLED)
	{
		Rotate(&a,angle);
		Rotate(&b,angle);
		Rotate(&c,angle);
		Rotate(&d,angle);
	}


	a.x+=center->x;
	a.y+=center->y;
	b.x+=center->x;
	b.y+=center->y;		
	c.x+=center->x;
	c.y+=center->y;
	d.x+=center->x;
	d.y+=center->y;



	DrawMapXLine(a,b,color);
	DrawMapXLine(b,c,color);
	DrawMapXLine(c,d,color);
	DrawMapXLine(d,a,color);
	
	//printf("drew diamond at (%li,%li)\n",center->x,center->y);

}

/* ========================================================================
   Function    - DrawMapTriangle
   Description - draws a triangle from a given center and radius
   Returns     - void
   ======================================================================== */
static void DrawMapTriangle( POINT *c,LONG angle,LONG radius,ULONG color)
{

	POINT l,r,t;		//  t
						// l r
	
	
	t.x=0;
	t.y=0+radius*3;

	l.x=0-radius;
	l.y=0-radius;

	r.x=0+radius;
	r.y=0-radius;

	
	//rotate to the object's angle
	if (angle!=MAP_NOT_ANGLED)
	{
		Rotate(&l,angle);
		Rotate(&r,angle);
		Rotate(&t,angle);
	}

	//put it in map coords
	t.x+=c->x;
	t.y+=c->y;
	l.x+=c->x;
	l.y+=c->y;
	r.x+=c->x;
	r.y+=c->y;

	DrawMapXLine(r,l,color);
	DrawMapXLine(t,r,color);
	DrawMapXLine(l,t,color);

}

/* ========================================================================
   Function    - DrawMapOctagon
   Description - draws an octagon from a given center and radius
   Returns     - void
   ======================================================================== */
static void DrawMapOctagon( POINT *center,LONG angle,LONG radius,ULONG color)
{
                        //   t
    POINT a,b,c,d,      //  a b
            e,f,g,h;    // h   c
    POINT o,t;          //   o
                        // g   d
                        //  f e

	const LONG HalfRadius = radius/2;
					
						//o and t are for "angled" octogons
						//to show which way it is pointing.

	//create the octagon		
	a.x=-HalfRadius;
	a.y=radius;

	b.x=HalfRadius;
	b.y=radius;

	c.x=radius;
	c.y=HalfRadius;
	
	d.x=radius;
	d.y=-HalfRadius;

	e.x=HalfRadius;
	e.y=-radius;

	f.x=-HalfRadius;
	f.y=-radius;

	g.x=-radius;
	g.y=-HalfRadius;
	
	h.x=-radius;
	h.y=HalfRadius;

	o.x=0;
	o.y=0;

	t.x=0;
	t.y=(radius*1.25);


	//rotate to the object's angle
	if (angle!=MAP_NOT_ANGLED)
	{
		Rotate(&a,angle);
		Rotate(&b,angle);
		Rotate(&c,angle);
		Rotate(&d,angle);
		Rotate(&e,angle);
		Rotate(&f,angle);
		Rotate(&g,angle);
		Rotate(&h,angle);
	}

	//put it in map coords
	a.x+=center->x;
	a.y+=center->y;
	b.x+=center->x;
	b.y+=center->y;
	c.x+=center->x;
	c.y+=center->y;
	d.x+=center->x;
	d.y+=center->y;
	e.x+=center->x;
	e.y+=center->y;
	f.x+=center->x;
	f.y+=center->y;
	g.x+=center->x;
	g.y+=center->y;
	h.x+=center->x;
	h.y+=center->y;

	DrawMapXLine(a,b,color);
	DrawMapXLine(b,c,color);
	DrawMapXLine(c,d,color);
	DrawMapXLine(d,e,color);
	DrawMapXLine(e,f,color);
	DrawMapXLine(f,g,color);
	DrawMapXLine(g,h,color);
	DrawMapXLine(h,a,color);

	//draw a line showing the angle (o -> t)
	if (angle!=MAP_NOT_ANGLED)
	{
		Rotate(&o,angle);
		Rotate(&t,angle);
		o.x+=center->x;
		o.y+=center->y;
		t.x+=center->x;
		t.y+=center->y;
		DrawMapXLine(o,t,color);
	}
}

/* ========================================================================
   Function    - DrawMapDiviningRod
   Description - Draws a Divining Rod shaped pointer on the map
   Returns     - void
   ======================================================================== */
static void DrawMapDiviningRod( POINT *c,LONG angle,LONG radius,ULONG color)
{

    POINT l,r,t;        //  t
                        //  |
                        //  |
                        // / \  (this message is here to supress a warning)
                        //l   r

	t.x=0;
	t.y=0+radius*2.5;

	l.x=0-radius;
	l.y=0-radius;

	r.x=0+radius;
	r.y=0-radius;

	
	//rotate to the object's angle
	if (angle!=MAP_NOT_ANGLED)
	{
		Rotate(&l,angle);
		Rotate(&r,angle);
		Rotate(&t,angle);
	}

	//put it in map coords
	t.x+=c->x;
	t.y+=c->y;
	l.x+=c->x;
	l.y+=c->y;
	r.x+=c->x;
	r.y+=c->y;

	DrawMapXLine(*c,l,color);
	DrawMapXLine(*c,r,color);
	DrawMapXLine(*c,t,color);

}


	
/* ========================================================================
   Function    - DrawMapGrid
   Description - Draws the grid according to the blockmap
   Returns     - void
   ======================================================================== */
static void DrawMapGrid(void)
{
	long bx,by;

	POINT a,b;
	

	//draw the vertical lines
	for (bx=0;bx<=blockm_header.cols;++bx)
	{
			a.x=blockm_header.xo+128*bx;
			a.y=blockm_header.yo;
			b.x=blockm_header.xo+128*bx;
			b.y=blockm_header.yo+blockm_header.rows*128;
			DrawMapXLine(a,b,197);
	}	

	//draw the horizontal lines
	for (by=0;by<=blockm_header.rows;++by)
	{
			a.x=blockm_header.xo;
			a.y=blockm_header.yo+128*by;
			b.x=blockm_header.xo+blockm_header.cols*128;
			b.y=blockm_header.yo+128*by;
			DrawMapXLine(a,b,197);
	}	



}


/* =======================================================================
   Function    - MapHandleThings
   Description - Draws things that aren't taken care of by the game
   Returns     - void
   ======================================================================== */
static void MapHandleThings(void)
{
	LONG t;
	
	
	
	if (gMapInfo.fDrawAll)
	{	
		for (t=0;t<MaxThingLoaded;++t)
		{
			if (mythings[t].valid)
					if (THING_IN_BNDBOX(t)) 
						if (mythings[t].type > LAST_GAME_AVATAR)
							DrawMapThing(t,MAP_NOT_ANGLED);
		}
		return;
	}
	
	for (t=0;t<MaxThingLoaded;++t)
	{
		if(mythings[t].valid)
			if (THING_IN_BNDBOX(t)) 
				if (mythings[t].type > LAST_GAME_AVATAR)
				{
					if (IS_PLANT(t))
						DrawMapThing(t,MAP_NOT_ANGLED);
					else if (IS_TREASURE(t) && (mythings[t].dist>>8)<gMapInfo.Highlight.Treasure)
						DrawMapThing(t,MAP_NOT_ANGLED);
					else if (IS_MAGIC(t) && (mythings[t].dist>>8)<gMapInfo.Highlight.Magic)
						DrawMapThing(t,MAP_NOT_ANGLED);
					else if (IS_EVIL(t) && (mythings[t].dist>>8)<gMapInfo.Highlight.Evil)
						DrawMapThing(t,MAP_NOT_ANGLED);
				
					//enemies are drawn by the gMapInfo.GameDrawFunc callback
				}
	}
	
}






/* ========================================================================
   Function    - MapHandleSecretSSect
   Description - makes sure the map is able to draw all segs of the ssect
   				 you are in
   Returns     - void
   ======================================================================== */
static void MapHandleSecretSSect(void)
{
	LONG i;
	LONG o;
	LONG n;

	ULONG ssect=find_ssector((gMapInfo.WorldCenter.x),(gMapInfo.WorldCenter.y));

	o=ssectors[ssect].o;
	n=o+ssectors[ssect].n;

	
	for (i=o;i<n;++i)
	{
		if (gMapInfo.fColorCycleSSect)
		{
			POINT a,b;
			a.x=vertexs[segs[i].a].x;	//outline the ssect by redrawing
			a.y=vertexs[segs[i].a].y;	//segs in a different color
			b.x=vertexs[segs[i].b].x;
			b.y=vertexs[segs[i].b].y;

			DrawMapXLine(a,b,frames%256);	//make the segs color-cycle
		}
		
		linedefs[segs[i].lptr].flags &= (~NOT_ON_MAP_BIT);

	}
}

void MapHandleZoom(LONG action)
{
	static LONG OldZoom=2;
	static LONG OldReductionCount=0;


	if(action==SERVICE_ZOOM && gMapInfo.fFullScreen)
	{
		if (OldReductionCount==GetMarginReductionCount())
			return;
		
		MapHandleZoom(FULL_LEVEL_ZOOM);	//safe recursion

	}
		
	else if (action==FULL_LEVEL_ZOOM)
	{
		LONG Wadx,Wady,Wadw,Wadh;
		LONG r,l,t,b;
		LONG NewZoomY,NewZoomX;
		


		get_margin_size(&r,&l,&t,&b);
		// TODO: (fire lizard) uncomment
		//CalcWadBounds(&Wadx,&Wady,&Wadw,&Wadh);
		
		NewZoomX=Wadw/(MAX_VIEW_WIDTH-r-l)+1;
		NewZoomY=Wadh/(MAX_VIEW_HEIGHT-b-t)+1;
		OldZoom=gMapInfo.ZoomFactor;
		gMapInfo.ZoomFactor=MAX(NewZoomX,NewZoomY);

		gMapInfo.WorldCenter.x=Wadw/2+Wadx;
		gMapInfo.WorldCenter.y=Wadh/2+Wady;
		
	}
	else
	{
		gMapInfo.ZoomFactor=OldZoom;
	}

	
	OldReductionCount=GetMarginReductionCount();		
	
	
}


/* =======================================================================
   Function    - ZoomMap
   Description - changes the map's zoom level
   Returns     - void
   ======================================================================== */
void ZoomMap( LONG unused, LONG f )
{
	if(!gMapInfo.fDrawMap)
		return;

	if(f>0)
	{
		--gMapInfo.ZoomFactor;
		if(gMapInfo.ZoomFactor<2)
			gMapInfo.ZoomFactor=2;
	}
	else
	{
		++gMapInfo.ZoomFactor;
		if(gMapInfo.ZoomFactor>MAX_ZOOM_FACTOR)
			gMapInfo.ZoomFactor=MAX_ZOOM_FACTOR;
	}
}

/* =======================================================================
   Function    - ZoomMapAbsolute
   Description - changes the map's zoom level
   Returns     - void
   ======================================================================== */
void ZoomMapAbsolute( LONG unused, LONG f )
{
	if(!gMapInfo.fDrawMap)
		return;

	gMapInfo.ZoomFactor=f;
	if(gMapInfo.ZoomFactor<2)
		gMapInfo.ZoomFactor=2;
}



/* ========================================================================
   Function    - MapIsActive
   Description - checks if the map is being drawn or not.
   Returns     - TRUE if it is being drawn
   ======================================================================== */
BOOL MapIsActive(void)
{
	return (BOOL)gMapInfo.fDrawMap;
}


/* ========================================================================
   Function    - MapZoomFactor
   Description - returns the zoom factor of the map
   Returns     - returns the zoom factor of the map
   ======================================================================== */
LONG MapZoomFactor(void)
{
	return gMapInfo.ZoomFactor;
}

LONG MapCenterX(void)
{
	return gMapInfo.ScreenCenter.x;
}

LONG MapCenterY(void)
{
	return gMapInfo.ScreenCenter.y;
}

void SetMapCenter(LONG x,LONG y)
{
	gMapInfo.ScreenCenter.x=x;
	gMapInfo.ScreenCenter.y=y;
}





/* ========================================================================
   Function - CalcWadBounds
   Description - calculates the xy bounds of the wad
   Returns - void
   ======================================================================== */
void CalcWadBounds(LONG *rx, LONG *ry,LONG *rw,LONG *rh)
{
	
	//SHORT* TestBlock=NULL;
	//LONG x=0,y=0,w=0,h=0;
	
	*rx=blockm_header.xo-MAP_WAD_BOUNDS_BUFFER;
	*ry=blockm_header.yo-MAP_WAD_BOUNDS_BUFFER;
	*rw=blockm_header.cols*128+MAP_WAD_BOUNDS_BUFFER;
	*rh=blockm_header.rows*128+MAP_WAD_BOUNDS_BUFFER;
}

	
	
