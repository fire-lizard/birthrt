/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: DynamTex.h		--Declarations and stuff for Dynamtex.c
   Author: Wes Cumberland
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#ifndef DYNAMTEX_H
#define DYNAMTEX_H


// GWP off for BR1. 
#define WRC_CAMERA_TEST 00

#if WRC_CAMERA_TEST

#define DYNAMIC_TEXTURE_MASK 0x08
#define CAMERA_TYPE 19

typedef struct			
{
	char name[9];
	LONG w,h;
	SHORT buffer;
}	BufferInfo;


typedef	CAMERA CameraDescriptor;

typedef struct 
{
	PTR screen;
	long width;
	long height;
} VIEW_INFO;

typedef struct
{
	long LDidx;
	SHORT BufferType;
} DynamicTexture;


/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void CrossRefCameras(void);
void NewCameraThing(LONG t);
void NewCameraLinedef(LONG l);
void RenderCameraView(SHORT DynTexIdx,SHORT CameraHdl);
void NewVideoBuffer(SHORT);
void RedirectVideo(SHORT );
void RestoreVideo(void);

void DTCheckLinedefs(void);

SHORT TexNametoCamBufHandle(char*);
LONG  TexNametoCamBufWidth(char*);
LONG  TexNametoCamBufHeight(char*);
void  ResetDynTexDrawn(void);
void  ResetDynTexFrameData(void);
LONG  RenderCameras(void);


extern CameraDescriptor Cameras[];
extern long CurrCam;
extern DynamicTexture TempCameraLinedefTable[];
extern DynamicTexture DynamicTextures[];
extern LONG CurrDynTex;
extern BOOL VideoCurrentlyRedirected;
extern BufferInfo CamBufs[];
extern char DynTexDrawn[];
extern char DynTexFrameData[];

#endif // WRC_CAMERA_TEST
#endif

