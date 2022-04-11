/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: DynamTex.c		-Allows non-static pictures to be shown on walls
   Author: Wes Cumberland
   ========================================================================
   Contains the following internal functions: 

   NewVideoBuffer 			-creates a new video buffer to render into
   CloseVideoBuffer			-frees a video buffer
   RedirectVideo  			-redirects rendering to a buffer
   RestoreVideo				-restores rendering to the screen

   
   Contains the following general functions:

   TexNametoCamBufHandle	-Finds the appropriate buffer for a dynam. texture
   TexNametoCamBufWidth		-Finds the appropriate width for a dynam. texture
   TexNametoCamBufHeight	-Finds the appropriate height for a dynam. texture
   ResetDynTexDrawn			-Resets the DynTexDrawn table
   ResetDynTexFrameData		-Resets the DynTexFrameData table
   CrossRefCameras			-Associates dynamic textures with cameras
   NewCameraThing 			-Create a new camera in the Cameras table
   NewCameraLinedef			-Create a new Dynamic Texture in the DT table
   DTCheckLinedefs			-Checks the linedefs for Dynamic Textures
   RenderCameraView			-Renders the view from a Camera in the level
   RenderCameras			-Checks which cams need rendering and renders them 


   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#if WRC_CAMERA_TEST

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"
#include "engine.h"
#include "engint.h"
#include "machine.h"
#include "dynamtex.h"
/* ------------------------------------------------------------------------
   Defines and typedefs
   ------------------------------------------------------------------------ */
#define MAX_LEVEL_CAMERAS 12
#define MAX_DYNAMIC_TEXTURES 120
#define MAX_DYNTEX_BUFFERS 4

#define CAMERA128x128 1			//unused
#define CAMERA128x256 2			//unused
#define CAMERA256x128 3			//unused
#define CAMERA256x256 4			//unused


/* ------------------------------------------------------------------------
   Globals
   ------------------------------------------------------------------------ */
extern LONG first;

BufferInfo CamBufs[MAX_DYNTEX_BUFFERS]={{"CAMERA11",128,128,-1},
											   {"CAMERA12",128,256,-1},
											   {"CAMERA21",256,128,-1},
											   {"CAMERA22",256,256,-1}};


char DynTexDrawn[MAX_TEXTURES];			//unused

char DynTexFrameData[MAX_TEXTURES];		//do we need to render this Dynamic-
										//Texture for next frame?

CameraDescriptor Cameras[MAX_LEVEL_CAMERAS];//Camera table
LONG CurrCam=0;								//where we are in the Camera table

DynamicTexture DynamicTextures[MAX_DYNAMIC_TEXTURES];//Dynamic Texture table
LONG CurrDynTex=0;									//where we are in the
													//Dynamic Texture table

BOOL VideoCurrentlyRedirected=FALSE;			//are we currently drawing
												//to a buffer?



/* ========================================================================
   Function    - TexNametoCamBufHandle
   Description - Finds which camera buffer to use based on the texture name
   Returns     - the Handle to camera's buffer
   ======================================================================== */
SHORT TexNametoCamBufHandle(char *tname)
{
	long i;

	for (i=0;i<MAX_DYNTEX_BUFFERS;i++)
	{
		if (!strncmp(tname,CamBufs[i].name,8))
			return CamBufs[i].buffer;
	}
	return -1;
}

/* ========================================================================
   Function    - TexNametoCamBufWidth
   Description - Finds the width of the CamBuf of the given texture name
   Returns     - the width as a long
   ======================================================================== */
LONG TexNametoCamBufWidth(char *tname)
{
	long i;

	for (i=0;i<MAX_DYNTEX_BUFFERS;i++)
	{
		if (!strncmp(tname,CamBufs[i].name,8))
			return CamBufs[i].w;
	}
	return -1;
}

/* ========================================================================
   Function    - TexNametoCamBufHeight
   Description - Finds the height of the CamBuf of the given texture name
   Returns     - the height as a long
   ======================================================================== */
LONG TexNametoCamBufHeight(char *tname)
{
	long i;

	for (i=0;i<MAX_DYNTEX_BUFFERS;i++)
	{
		if (!strncmp(tname,CamBufs[i].name,8))
			return CamBufs[i].h;
	}
	return -1;
}


/* ========================================================================
   Function    - ResetDynTexDrawn
   Description - clears the DynTexDrawn table	(unused)
   Returns     - nut'n honey.
   ======================================================================== */
void ResetDynTexDrawn(void)
{
	memset(DynTexDrawn,0,MAX_TEXTURES);
}	

/* ========================================================================
   Function    - ResetDynTexFrameData
   Description - Sets the DynTexFrameData table to all -1s
   Returns     - void
   ======================================================================== */
void ResetDynTexFrameData(void)
{
	long i;
	for (i=0;i<MAX_TEXTURES;i++)
	{
		DynTexFrameData[i]=-1;
	}
		
//	memset(DynTexFrameData,-1,MAX_TEXTURES);	//does this work?
}	


/* ========================================================================
   Function    - CrossRefCameras
   Description - After loading all the DynamicTextures and Camera objects
                 call this to associate the linedefs and cameras
   Returns     - void
   ======================================================================== */
void CrossRefCameras(void)
{
	LONG i=0,j=0,LDtemp;
//	printf("Entered CrossRefCameras CurrCam=%li, CurrDynTex=%li\n",CurrCam,CurrDynTex);


	for (i=0;i<CurrCam;i++)
		for(j=0;j<CurrDynTex;j++)
		{
			LDtemp=DynamicTextures[j].LDidx;
//			printf("checking sectors (camera %li vs dtex %li) %li vs %li\n",i,j,point_to_sector(CAMERA_INT_VAL(Cameras[i].x),CAMERA_INT_VAL(Cameras[i].y)),tag_to_sector(linedefs[LDtemp].tag));
			if (point_to_sector(CAMERA_INT_VAL(Cameras[i].x),CAMERA_INT_VAL(Cameras[i].y))==tag_to_sector(linedefs[LDtemp].tag))
			{
//				printf("cross reffing Camera %li (sector %li) with DTex %li (linedef %li)\n",i,point_to_sector(CAMERA_INT_VAL(Cameras[i].x),CAMERA_INT_VAL(Cameras[i].y)),j,DynamicTextures[j].LDidx);
				linedefs[LDtemp].tag=i;
				linedefs[LDtemp].special=j;
			}
		}

}

/* ========================================================================
   Function    - NewCameraThing
   Description - Creates a new camera in the camera table
   Returns     - void
   ======================================================================== */
void NewCameraThing(LONG t)
{
	long ta;

	if (CurrCam>MAX_LEVEL_CAMERAS)
		fatal_error("\n too many Cameras %s(%li)\n",__FILE__,__LINE__);

//	printf("found Camera in Sector %li\n",point_to_sector(things[t].x,things[t].y));

	Cameras[CurrCam].x=things[t].x << CAMERA_FIXEDPT;
	Cameras[CurrCam].y=things[t].y << CAMERA_FIXEDPT;
	Cameras[CurrCam].z=point_to_floor_height(things[t].x,things[t].y)+80;
	Cameras[CurrCam].p=0;
	

    //there's gotta be an easier way to convert angles than this!

	ta=((0xffff/360)*things[t].angle)/256;
	ta=(ta-63)&0xff;
	ta=(65535-ta)&0xFF;

	ta+=1;  //all the converted angles are 1 less than they should be...


	Cameras[CurrCam++].a=ta;
	
	//printf("converted %i to %li\n",things[t].angle,Cameras[CurrCam-1].a);		   
	//while (!key_status(KEY_SPACE));

}




/* ========================================================================
   Function    - NewCameraLinedef
   Description - Creates a new Dynamic Texture in the DynamicTextures table
   Returns     - void
   ======================================================================== */
void NewCameraLinedef(LONG l)
{
	LONG i;

	if (CurrDynTex>MAX_DYNAMIC_TEXTURES)
		fatal_error("\n too many dynamic Textures %s(%li)\n",__FILE__,__LINE__);

		
	DynamicTextures[CurrDynTex].LDidx=l;

	for (i=0;i<MAX_DYNTEX_BUFFERS;i++)		//find which buffer to use
	{
		if (!strncmp(sidedefs[linedefs[l].psdb].n3,CamBufs[i].name,8))
		{
			DynamicTextures[CurrDynTex].BufferType=i;
			break;
		}
	}

	if (i==MAX_DYNTEX_BUFFERS)
		fatal_error("\n invalid camera type %s(%li)\n",__FILE__,__LINE__);
		

	if(CamBufs[DynamicTextures[CurrDynTex].BufferType].buffer==-1)
		NewVideoBuffer(DynamicTextures[CurrDynTex].BufferType);
	
		
	CurrDynTex++;

}

/* ========================================================================
   Function    - DTCheckLinedefs
   Description - Checks all the linedefs and calls NewCameraLinedef on the 
                 ones that have Dynamic Textures on their first sidedef
   Returns     - void
   ======================================================================== */
void DTCheckLinedefs(void)
{
	LONG i;

	for (i=0;i<tot_linedefs;i++)
	{
//		printf("checking linedef %li for Dynamic Textureness (%-6.6s)\n",i,sidedefs[linedefs[i].psdb].n3);
		if (!strncmp(sidedefs[linedefs[i].psdb].n3,"CAMERA",6))
		{
//			printf("calling NewCameraLinedef\n");
			NewCameraLinedef(i);
		}
	}
}
	

/* ========================================================================
   Function    - RenderCameras
   Description - Checks the DynTexFrameData Table to see if a camera needs 
                 to rendered, and if it does, it renders it.
                 It then resets the table for you.
   Returns     - the number of cameras rendered
   ======================================================================== */
LONG RenderCameras(void)
{
	long count=0;
	long i=0;

	static LONG FirstRend=1;

	if (FirstRend)
	{
		ResetDynTexFrameData();
		FirstRend=0;
	}


	while(i<MAX_TEXTURES)
	{
		if (DynTexFrameData[i]!=-1)
		{
//			printf("Calling RCV(%li,%li)",linedefs[DynTexFrameData[i]].special,
//						linedefs[DynTexFrameData[i]].tag);

			RenderCameraView(linedefs[DynTexFrameData[i]].special,
							linedefs[DynTexFrameData[i]].tag);
			count++;
		}
		i++;
	}
	
	ResetDynTexFrameData();

	return count;
}





/* ========================================================================
   Function    - RenderCameraView
   Description - Renders the view from a specified camera and sticks the 
                 data into the buffer of the given DynamicTexture 
   Returns     - void!
   ======================================================================== */
void RenderCameraView(SHORT DynTexIdx,SHORT CameraHdl)
{
	
	CAMERA OldCam=camera;
//	SHORT BufferType=DynamicTextures[DynTexIdx].BufferType;

//	printf("rendering Camera %li to DynamTex %li (%s)(%lix%li) (buffer: %li)\n",CameraHdl,DynTexIdx,
//		CamBufs[DynamicTextures[DynTexIdx].BufferType].name,
//		CamBufs[DynamicTextures[DynTexIdx].BufferType].w,
//		CamBufs[DynamicTextures[DynTexIdx].BufferType].h,
//		CamBufs[DynamicTextures[DynTexIdx].BufferType].buffer);

	//printf("camera %li facing %li\n",CameraHdl,Cameras[CameraHdl].a);



	
	if (CameraHdl>CurrCam)
		fatal_error("Invalid Camera %li specified! %s(%li)\n",CameraHdl,__FILE__,__LINE__);
	
	camera=Cameras[CameraHdl];
	
	RedirectVideo(DynamicTextures[DynTexIdx].BufferType);

	set_view();

	clear_extents();
	clear_spans();
	clear_thing_spans();

	first=TRUE;

	draw_node(tot_nodes-1);

	draw_spans();

	draw_thing_spans();

	camera=OldCam;	   			//restore the old camera

	RestoreVideo();
	
}

/* =======================================================================
   Function    - NewVideoBuffer
   Description - creates a new buffer for a camera type
   Returns     - void
   ======================================================================== */
static void NewVideoBuffer(SHORT type)
{
	
	if (type>=MAX_DYNTEX_BUFFERS)
		fatal_error("too many camera types! %s(%li)\n",__FILE__,__LINE__);
	if (CamBufs[type].buffer!=-1)
		return;

	CamBufs[type].buffer=OpenBitm(CamBufs[type].w,CamBufs[type].h);		// sideways, as usual

}

/* =======================================================================
   Function    - RedirectVideo
   Description - Alters all the render stuff to render to a buffer other 
                 than the screen
   Returns     - void
   ======================================================================== */
static void RedirectVideo(SHORT BufferIdx)
{
	if (VideoCurrentlyRedirected)
		fatal_error("\nCan't render two cameras at once! %s(%li)\n",__FILE__,__LINE__);
	
	if (BufferIdx>MAX_DYNTEX_BUFFERS)
		fatal_error("\nInvalid Buffer %s(%li)\n",__FILE__,__LINE__);

	if (CamBufs[BufferIdx].buffer==-1)
		fatal_error("\nInvalid Buffer %s(%li)\n",__FILE__,__LINE__);


	SaveVideoSet();
	VideoCurrentlyRedirected=TRUE;
	screen_buffer_width=CamBufs[BufferIdx].w;
	screen_buffer_height=CamBufs[BufferIdx].h;

	screen=((PTR)BLKPTR(CamBufs[BufferIdx].buffer))+sizeof(BITMHDR);
	
//	printf("changed screen to (%lix%li)(%li)\n",CamBufs[BufferIdx].w,CamBufs[BufferIdx].h,CamBufs[BufferIdx].buffer);
	
	set_view_size(0,0,screen_buffer_width,screen_buffer_height);
	set_view();

}

/* =======================================================================
   Function    - RestoreVideo
   Description - Restores all the stuff that RedirectVideo changed
   Returns     - void
   ======================================================================== */
static void RestoreVideo(void)
{
	if (VideoCurrentlyRedirected)
	{
		VideoCurrentlyRedirected=FALSE;
		RestoreVideoSet();
		set_view_size(0,0,screen_buffer_width,screen_buffer_height);
		set_view();

	}
}

/* =======================================================================
   Function    - CloseVideoBuffer
   Description - Frees a camera's buffer
   Returns     - void
   ======================================================================== */
static void CloseVideoBuffer(SHORT VidIdx)
{
	if(VidIdx>MAX_DYNTEX_BUFFERS)
		fatal_error("can only close valid buffers in VidBuf (too many) %s(%li)\n",__FILE__,__LINE__);
	if(CamBufs[VidIdx].buffer==-1)
		fatal_error("can only close valid buffers in VidBuf (-1) %s(%li)\n",__FILE__,__LINE__);

	if (VideoCurrentlyRedirected)
		RestoreVideo();
	CloseBitm(CamBufs[VidIdx].buffer);
	VideoCurrentlyRedirected=FALSE;
}

/* =======================================================================
   Function    - CloseAllVideoBuffers
   Description - Frees all cameras' buffers
   Returns     - void
   ======================================================================== */
void CloseAllVideoBuffers(void)
{
	LONG i;
	for (i=0;i<MAX_DYNTEX_BUFFERS;i++)
		if (CamBufs[i].buffer!=-1)
			CloseVideoBuffer(i);
}


#endif // WRC_CAMERA_TEST

/*	======================================================================== */



