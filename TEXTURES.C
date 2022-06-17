/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: TEXTURES.C  -Handles textures
   Author: Chris Phillips
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:
   get_texture           -gets a texture from somewhere???
   get_floor_texture     -gets a floor texture from somewhere???
   scale_col_ttop        -???
   scale_col_tbot        -???
   scrape                -???
   hscrape               -???


   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <ctype.h>
#include "DEBUG.H"
#include "SYSTEM.H"
#include "ENGINE.H"
#include "ENGINT.H"
#include "MACHINE.H"
#include "DYNAMTEX.H"
#include "LIGHT.H"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

	//REVISIT:Clean up all get texture stuff.. fix sky problems.. split
	// out span stuff.. recode spans for speed. unify texture types...
	// god! I could go on and on......

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
// height expanded x1.43  (1/.70)
//#define SCRAPE_Y_FACTOR		70		// hacked back by Bob's request 8-29-96
#define SCRAPE_Y_FACTOR		100		// {55} JPC per Wes's request 8-08-96

//#define FIRST_LINE_TOP_OF_SCRAPE		46
#define FIRST_LINE_TOP_OF_SCRAPE		0

// BATTLE_WAD_ADJUST is used to move the sky panels up when the render
// window is smaller than full size, the way it is in battles.  This lets
// us see the 3-D layering effect of the panels.
#define BATTLE_WAD_ADJUST				48	// [d11-11-96 JPC]

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
void WriteErrFile(char *n);

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
TEXTURE	textures[MAX_TEXTURES];
long		last_texture = 0;
SHORT		hSky_filenames = fERROR;
#define  cSKY_TEXTURES		12				// [d11-08-96 JPC] used to be 11
TEXTURE	sky_textures[cSKY_TEXTURES];

BOOL     gfTestMagicLightSwitch;       // [d7-01-96 JPC] if TRUE, lighten things up
BOOL     gfInfraVision;                // [d7-02-96 JPC]
LONG		gcMagicTemporaryLight;			// [d7-02-96 JPC]
LONG		gcFrames;							// [d7-03-96 JPC] a simple frame counter

// [d7-11-96 JPC] extern	LONG	wall_scale;
extern	long	col_hei;          // height of the column we need to draw
                                 // --it is computed in RENDER.C as
                                 // ABS (ceiling height - floor height)
                                 // [JPC comment]
extern	PTR	pTexture;
extern	long	wTexture;
extern	long	hTexture;
extern	LONG	gPeggedValue;
extern	LONG	floor_z;
extern	LONG	gtxoff;
extern	LONG	tyoff;
extern	LONG	sin_table[];
#if WRC_CAMERA_TEST
extern	BOOL	VideoCurrentlyRedirected;
#endif
extern	LONG	frames;
extern   LONG  gLinedefLight;          // [d6-05-96 JPC] calculated in RENDER.C
extern	LONG	gAdjustLight;				// [d9-23-96 JPC] calculated in RENDER.C
extern   LONG  gMinLight;              // [d9-23-96 JPC] Minimum allowed value for light.
extern   LONG  gMaxLight;              // [d9-23-96 JPC] Maximum allowed value for light.
extern	LONG	gRemapIndex;				// [d7-29-96 JPC] experimental

/* ========================================================================
   Function    - get_texture
   Description - gets a texture from somewhere???
   Returns     - entry in the texture table where the new one was put
   Change      - 6-06-96 JPC: added o_status parameter so caller can
                 tell what happened
   ======================================================================== */
long get_texture (char *in, ULONG * o_status)
{
	LONG		i;
	ULONG		t;
	SHORT		tex;
	char		n[20];
	char		tn[80];
	PTR		tptr;

	// GWP Moved the test to the front of the fn.
	if(*in == '-')
	{
		*o_status = GT_NO_TEXTURE;       // [d6-06-96 JPC]
		return(0);
	}
	
	/* copy string into local variable */
	for(i=0;i<8;++i, ++in)
	{
		if (isupper(*in))
		{
			n[i]=*in;
		}
		else
		{
			n[i] = toupper(*in);
		}
	}
	
	n[8]=0;
	
	// GWP Moved to inside copy loop. strupr(n);

	/* scan for duplicate texture */
	for(i=0;i<last_texture;++i)
	{
		if(strcmp(n,textures[i].name)==0)
		{
			*o_status = GT_FOUND_TEXTURE;
			return(i);
		}
	}

	t=last_texture++;

	if(last_texture>MAX_TEXTURES)
	{
		fatal_error("Exceeded MAX_TEXTURES in get_texture");
	}
	textures[t].type = NORMAL_TEX;			/* not see-through, not sky */
	sprintf(textures[t].name,"%s",n);

	if (strstr(n,"SKY"))							/* handle sky textures */
	{
		textures[t].type = SKY_TEX;
		
		// in case nobody bothers to fill them in properly in the interim
		textures[t].t = fERROR;
		textures[t].w = 0xDC;
		textures[t].h = 0xDC;
		textures[t].scale = UNITARY_SCALE;
		textures[t].frameHandle = fERROR;
		
		*o_status = GT_SKY_TEXTURE;
		return(t);
	}
	

#if WRC_CAMERA_TEST
	if (!strncmp(n,"CAMERA",6))
	{
		textures[t].t = tex = TexNametoCamBufHandle(n);
		textures[t].w =       TexNametoCamBufWidth(n);
		textures[t].h =       TexNametoCamBufHeight(n);
		textures[t].scale = UNITARY_SCALE;
		textures[t].type = CAMERA_TEX;
//		printf("Found Camera Texture! (%lix%li)\n",textures[t].w,textures[t].h);
		*o_status = GT_CAMERA_TEXTURE;
		return t;
	}
#endif
	
	sprintf(tn,"%s%s.pcx",TEXTURE_PATH,n);
	// [d3-19-97 JPC] Now that we are using resource files,
	// we can't check whether the actual file exists.
	// if(Exists(tn))
	{
		printf("Loading texture %s\n", tn);
		textures[t].t = tex = GetResourceRot(tn);
		if (tex != fERROR)
		{
			BITMPTR	bptr;
			
			SetClass2(tex);
			textures[t].frameHandle = fERROR; // not animated
			bptr = (BITMPTR) BLKPTR(tex);
			textures[t].w = bptr->w;
			textures[t].h = bptr->h;
			textures[t].scale = bptr->scale;
			tptr = ((PTR)bptr) + sizeof(BITMHDR);
			if (tptr[0] == 0)								/* check for see-through */
				textures[t].type = TRANSP_TEX;
//!!!!!!!!!!!!!!!!!!!!
//			printf("Texture:%14s  iBlk:%3d  type:%d (0x%02X)\n",tn,tex,(tptr[0] == 0),tptr[0]);
//!!!!!!!!!!!!!!!!!!!!
         *o_status = GT_LOADED_TEXTURE;

// [d9-09-96 JPC] We need to remember the index numbers of switch textures
// so we can change them when the user presses the switch.
			if (strcmp (n, "BRIKCSW1") == 0)
				gSwitchTexture[0][0] = t;
			else if (strcmp (n, "BRIKCSW2") == 0)
				gSwitchTexture[0][1] = t;

			else if (strcmp (n, "BRIKESW1") == 0)
				gSwitchTexture[1][0] = t;
			else if (strcmp (n, "BRIKESW2") == 0)
				gSwitchTexture[1][1] = t;

			else if (strcmp (n, "DUNGCSW1") == 0)
				gSwitchTexture[2][0] = t;
			else if (strcmp (n, "DUNGCSW2") == 0)
				gSwitchTexture[2][1] = t;

			return(t);
		}
	}

	// If we get here, something went wrong with the texture load.
	// [MDB] could not find the texture so assign the generic one
	// write the texture name to the err file
	WriteErrFile(n);

	// [MDB] assign the generic texture
	sprintf(tn, "%s%s.pcx", TEXTURE_PATH,"genwall");
	textures[t].t = tex = GetResourceRot(tn);
	if (tex == fERROR)
		fatal_error("Unable to load texture %s in get_texture\n",tn);
	SetClass2(tex);
	textures[t].frameHandle = fERROR; // not animated
	
	// GWP Brackets to scope the bitmptr.
	{
		BITMPTR bptr = (BITMPTR) BLKPTR(tex);
		
		textures[t].w = bptr->w;
		textures[t].h = bptr->h;
		textures[t].scale = bptr->scale;
	}
   *o_status = GT_GENERIC_TEXTURE;
	return(t);
}

/* ========================================================================
   Function    - load_sky_textures
   Description
   Returns     - void
   ======================================================================== */
void load_sky_textures (void)
{
#define LST_DEFAULT_NAME "BACK01"

	int		iLastChar;
	ULONG		i;
	SHORT		tex;
	char		tn[80];
	char		szBaseName[80];

	strcpy (szBaseName, LST_DEFAULT_NAME"A");
	iLastChar = strlen (szBaseName) - 1;

	if (hSky_filenames != fERROR)
	{
		// If hSky_filenames is valid, then the "Panorama" section of the
		// scene file (*.SCN) specified the sky textures to use for elements
		// 0-9 of the array.  Element 10 is always the base name + "Z"
		// and element 11 is always the base name + "X."

		char *cpFileName;
		SHORT *phSkyFileNames;

		//GEH SetBlockAttr(hSky_filenames, LOCKED, LOCKED);
		//GEH phSkyFileNames = (SHORT *) BLKPTR(hSky_filenames);

		for (i=0; i<cSKY_TEXTURES; ++i)
		{
			phSkyFileNames = (SHORT *) BLKPTR(hSky_filenames);
			
			if (i == 10)
			{
				// Sky filler: grass on top 48, sky on bottom 208.
				// We no longer use the grass on the top 48, but it never
				// shows up so we're ignoring it.
				// cpFileName = "BACK01Z";
				strcpy (cpFileName, szBaseName);
				cpFileName[iLastChar] = 'Z';
			}
			else if (i == 11)
			{
				// Ground section (grass, rock, swamp, as specified by
				// the base file name).
				// cpFileName = "BACK01X";		// [d11-08-96 JPC] all grass
				strcpy (cpFileName, szBaseName);
				cpFileName[iLastChar] = 'X';
			}
			else if (phSkyFileNames[i] != fERROR)
			{
			    SHORT hSkyTextureName;
			
			    hSkyTextureName = phSkyFileNames[i];
				
				cpFileName = (char *) BLKPTR(hSkyTextureName);
				if (strlen (cpFileName) > 0)
				{
					strcpy (szBaseName, cpFileName);
					iLastChar = strlen (cpFileName) - 1;
				}
			}
			else
			{
				cpFileName = LST_DEFAULT_NAME"A";			// default texture
			}

			sprintf(tn,"%s%s.pcx",TEXTURE_PATH, cpFileName);

			printf("Loaded sky texture %s\n", tn);

			// Don't rotate the sky ceilings.
				sky_textures[i].t = tex = GetResourceStd(tn, FALSE);
			//	sky_textures[i].t = tex = GetResourceRot(tn);
			
			if (tex == fERROR)
				fatal_error("Unable to load texture %s in load_sky_textures (1)\n",tn);
			SetClass2(tex);
			sky_textures[i].iFrame = fERROR;
			
			// GWP Braces to scope the bitmptr.
			{
				BITMPTR bptr = (BITMPTR) BLKPTR(tex);
				
				sky_textures[i].w = bptr->h;
				sky_textures[i].h = bptr->w;
			}
		}
		//GEH ClrLock(hSky_filenames);
	}
	else
	{
		// Use default sky, upper sky, and ground textures
		// (BACK01A, BACK01Z, and BACK01X).
		for (i=0; i<cSKY_TEXTURES; ++i)
		{
			if (i==10)								// lower texture
				sprintf(tn,"%s%s.pcx",TEXTURE_PATH, LST_DEFAULT_NAME"Z");
			else if (i == 11)
				sprintf(tn,"%s%s.pcx",TEXTURE_PATH, LST_DEFAULT_NAME"X"); // [d11-08-96 JPC] grass
			else
				sprintf(tn,"%s%s.pcx",TEXTURE_PATH, LST_DEFAULT_NAME"A");	// default texture

			// Don't rotate the sky ceilings.
				sky_textures[i].t = tex = GetResourceStd(tn,FALSE);
			//	sky_textures[i].t = tex = GetResourceRot(tn);
			
			if (tex == fERROR)
				fatal_error("Unable to load texture %s in load_sky_textures (2)\n",tn);
			SetClass2(tex);
			sky_textures[i].iFrame = fERROR;
			// GWP Braces to scope the bitmptr.
			{
				BITMPTR bptr = (BITMPTR) BLKPTR(tex);
				
				sky_textures[i].w = bptr->h;
				sky_textures[i].h = bptr->w;
			}
		}
	}
	
}

/* ========================================================================
   Function    - get_floor_texture
   Description - gets a floor texture from somewhere???
   Returns     - the entry in the floor texture table where it was stored
   ======================================================================== */
long get_floor_texture(char *in)
{
	long		i;
	ULONG		t;
	ULONG		status;                    // [d6-06-96 JPC]
	SHORT		tex;
	char		n[20];
	char		tn[80];

	for(i=0;i<9;++i)
		n[i]=in[i];

	n[8]=0;

	if(n[0]=='-')
		return(0);

	strupr(n);
	for(i=0;i<last_texture;++i)
	{
		if(strcmp(n,textures[i].name)==0)
			return(i);
	}

	// [d12-04-96 JPC] Check for sky must come BEFORE we increment last_texture.
	if (strstr(n,"SKY"))
		return (get_texture(in, &status));

	t=last_texture++;

	if(last_texture>=MAX_TEXTURES)
	{
		fatal_error("Exceeded MAX_TEXTURES in get_floor_texture");
	}

	// [d12-04-96 JPC] Old location of SKY code; moved it up.
	// if (strstr(n,"SKY"))
	// 	return (get_texture(in, &status));

	sprintf(textures[t].name,"%s",n);

	sprintf(tn,"%s%s.pcx",TEXTURE_PATH,n);

	// if(Exists(tn))
	{
		// Hard-code the names of animated textures here.
		if(strcmp (n, "F_WATR01") == 0 ||
			strcmp (n, "F_WATR02") == 0 ||
			strcmp (n, "F_WATR03") == 0 ||
			strcmp (n, "F_WATR04") == 0 ||
			strcmp (n, "F_WATR05") == 0 ||
			strcmp (n, "F_LAVA01") == 0 ||
			strcmp (n, "F_LAVA02") == 0 ||
			strcmp (n, "F_ACID01") == 0)
		{
			// Animated texture.  Load a dummy texture of size
			// 128 X 128 (drops to 64 X 64 when loaded), and put it
			// in t; also load the 256 X 256 texture (drops to 128 X 128
			// when loaded) and store handle in "frameHandle."  At the
			// beginning of each render, update the animated textures
			// according to the iFrame member by copying from
			// framehandle to t.  (See RENDER: ChangeTextureFrame.)

// [d11-18-96 JPC] Switch to separate created-in-memory resources.
// (The one loaded from disk restricted us to one animated texture
// per WAD--specifically, the last one loaded.)
			char dummy[80];
			// Load a 128 x 128 texture dummy (name is hard coded).
			sprintf(dummy,"%s.ant", n);
			textures[t].t = tex = GetResourceRot(dummy);
			if (tex != fERROR)
			{
				// Load the 256 X 256 texture with the four 128 x 128 frames:
				textures[t].frameHandle = GetResourceRot(tn);
				if (textures[t].frameHandle != fERROR)
				{
					SetClass2(tex);
					SetClass2(textures[t].frameHandle);
					SetModifyResource(tex);
					textures[t].iFrame = 0;
					
					// GWP Braces to scope the bitmptr
					{
						BITMPTR bptr = (BITMPTR) BLKPTR(tex);
						
						textures[t].w = bptr->w;
						textures[t].h = bptr->h;
						textures[t].scale = bptr->scale;
					}
					
					return t;
				}
				else
				{
					// Free dummy texture to make room for generic texture.
					DisposBlock (textures[t].t);
					textures[t].t = fERROR;
				}
			}
		}
		else
		{
			// Not an animated texture.
			textures[t].t = tex = GetResourceRot(tn);
			if (tex != fERROR)				// if error, load generic
			{
				SetClass2(tex);
				textures[t].iFrame = fERROR;
				// GWP Braces to scope the bitmptr
				{
					BITMPTR bptr = (BITMPTR) BLKPTR(tex);
					
					textures[t].w = bptr->w;
					textures[t].h = bptr->h;
				}
				textures[t].frameHandle = fERROR; // not animated
				// Look for homogenous floor textures. (Can be drawn without scaling.)
				
				   // Battlefield textures.
				   // If you get close to these textures it looks awful!
				if (strstr(n,"F_PLN01A") ||
				    strstr(n,"F_PLN02A") ||
				    strstr(n,"F_PLN03A") ||
				    strstr(n,"F_PLN04A") ||
				    strstr(n,"F_PLN05A") ||
				    strstr(n,"F_MTN01A") ||	
				    strstr(n,"F_MTN02A") ||
				    strstr(n,"F_MTN03A") ||
				    strstr(n,"F_MTN04A") ||
				    strstr(n,"F_FRS01A") ||
				    strstr(n,"F_FRS01B") ||
				    strstr(n,"F_FRS02A") ||
				    strstr(n,"F_FRS03A")
				
				    ||
				    // Adventure textures.
				    strstr(n,"F_CARP01") ||
				    strstr(n,"F_CARP02") ||
				    strstr(n,"F_DIRT01") ||
				    strstr(n,"F_DIRT02") ||
				    strstr(n,"F_GRAS01") ||
				    strstr(n,"F_GRAS02")
				    )
			    {
			    	textures[t].type = HOMOGENOUS_TEX;
			    }
				return(t);
			}
		}
	}

	// [MDB] could not find the texture so assign the generic one
	// write the texture name to the err file
	WriteErrFile(n);

	// [MDB] assign the generic texture
	sprintf(tn, "%s%s.pcx", TEXTURE_PATH,"genflr");
	textures[t].t = tex = GetResourceRot(tn);
	SetClass2(tex);
	textures[t].iFrame = fERROR;
	textures[t].frameHandle = fERROR; // not animated
	// GWP Braces to scope the bitmptr
	{
		BITMPTR bptr = (BITMPTR) BLKPTR(tex);
		
		textures[t].w = bptr->w;
		textures[t].h = bptr->h;
	}
	printf("Loaded floor texture %s\n", tn);
	return(t);
}

/* ========================================================================
   Function    - purge_all_textures
   Description -
   Returns     - void
   ======================================================================== */
void purge_all_textures (void)
{
	/* actual purge handled by PurgeClass(CLASS2) */
	
	// Purge all the ones we know about, and let the CLASS2 catch any lost
	// ones. Because here, we know what the resource id is.
	LONG i;
	for (i = 0; i < last_texture && i < MAX_TEXTURES; ++i)
	{
		if (textures[i].t > 0)
		{
			SetPurge(textures[i].t);
			// Clear the CLASS2 flag so we don't do this again.
			SetClassPerm(textures[i].t);
		}
		textures[i].t = fERROR;
		textures[i].frameHandle = fERROR;
	}
	
	for (i = 0; i < cSKY_TEXTURES; ++i)
	{
		if (sky_textures[i].t > 0)
		{
			SetPurge(sky_textures[i].t);
			// Clear the CLASS2 flag so we don't do this again.
			SetClassPerm(sky_textures[i].t);
		}
		sky_textures[i].t = fERROR;
		sky_textures[i].frameHandle = fERROR;
	}

	last_texture=0;
}

/* ========================================================================
   Function    - scale_col_ttop
   Description - ???
   Returns     - void
   ======================================================================== */
/* ------------------------------------------------------------------------
   Notes
	[JPC--deleted old note that described how it used to work.]

	A middle texture is always unpegged (RENDER: draw_wall sees to this).

	Upper textures can be "pegged high" or "unpegged."

	Lower textures can be "pegged low" or "unpegged."

	If an upper wall is pegged to the ceiling, then the bottom of the
	texture lines up with the bottom of the wall.

	If a lower wall is pegged to the floor, then the top of the texture lines
	up with the top of the wall.

	These are different from DOOM.
   ------------------------------------------------------------------------ */

/* ========================================================================
   Function    - scrape
   Description - draws the sky walls
   Returns     - void
   ======================================================================== */
void scrape(LONG x,			// column to draw in
			LONG yEnd,		// where to end drawing
			LONG yStart)	// yStart is where you start drawing
{
	ULONG 		wid = sky_textures[0].w;
	PTR			sptr;
	PTR	 		tptr;
	PTR			skyPtr;
	PTR	 		sptr_end;
	SHORT		panel = 0;
	ULONG		tsy = 0;
	ULONG		sptr_inc;
	//ULONG		tptr_inc = 1;		// = (SCRAPE_Y_FACTOR << 16) / 100;
	ULONG		tptr_inc = wid;		// = (SCRAPE_Y_FACTOR << 16) / 100;
	LONG 		margin_left, margin_right, margin_top, margin_bottom;
	ULONG		t_size = wid * sky_textures[0].h;
	// ULONG	t_size = wid <<16;

	// GWP for Low res skip scan lines.
#if !defined(_WINDOWS)
	if (fAIMoving)
	{
		if (x & 1)
			return;
	}
#endif
	
	++yEnd;
	get_margin_size(&margin_right,&margin_left,&margin_top,&margin_bottom);

#if WRC_CAMERA_TEST
	if (VideoCurrentlyRedirected)
	{
		sptr = (PTR)screen+((x*screen_buffer_height)+yStart);
		sptr_end = sptr + yEnd;
		sptr_inc = 1;
	}
	else
#endif
	{
		sptr = (PTR)screen+((yStart*screen_buffer_width)+x);
		sptr_end = (PTR)screen+((yEnd*screen_buffer_width));
		sptr_inc = screen_buffer_width;
	}

	// Now calculate the texture pointer by performing some exotic operations
	// on the screen x and y...

	if (!fHighRes)		// handle low res
	{
		x *= 2;
		tptr_inc *= 2;
	}
	
	x += camera.a * 15;
	if (!fHighRes)
	{
		x -= margin_left;
	}
	panel = x >> 8;
	// x &= 0xff;
	x &= (wid - 1);
	if (panel > 9)
		panel-=10;

	// Handle look up and down.
	yStart -= (camera.p * render_height) / MAX_VIEW_HEIGHT;

#if 01
	if (margin_bottom >= 164 /* BTLUI_PANEL_HEIGHT */ && margin_top == 0)
	{
		// [d11-11-96 JPC] Try to fix the battle wads so you can see the
		// mountains and so on in the background.
		// If less than full screen, just add a certain amount to y.
		// [d11-20-96 JPC] Added the margin_top test to allow for the chat
		// box that's sometimes at the top.
		if (!fHighRes)
			yStart += BATTLE_WAD_ADJUST/2;
		else
			yStart += BATTLE_WAD_ADJUST;
	}
#endif

	if (!fHighRes)
	{
		yStart *= 2;
		yStart -= margin_top;
	}

	// If yStart < 0, then we need to draw some sky above the sky panel.
	if (yStart < 0)
	{
		//tsy = wid + yStart;					// wid is 256 for all sky textures
		tsy = t_size - ((wid * yStart) + x);	// wid is 256 for all sky textures
		// textures[10] is BACK01Z.PCX, which has grass on top and sky
		// on bottom.
		skyPtr = ((PTR)BLKPTR((SHORT)sky_textures[10].t)) + sizeof(BITMHDR);
		//skyPtr = &skyPtr[x*wid];
		// Draw the last "yStart" lines of the generic sky texture
		// above our panel.
#if !defined(_WINDOWS)
		if (fAIMoving)
		{
			do {
				*(sptr+1)=*sptr = skyPtr[tsy];
				sptr += sptr_inc;
				tsy += tptr_inc;
			} while (tsy < wid);
		}
		else
#endif
		{
			do {
				*sptr = skyPtr[tsy];
				sptr += sptr_inc;
				tsy += tptr_inc;
			} while (tsy < wid);
		}
		yStart = 0;
	}

	// Now draw our panel and grass below it if necessary.
	tptr = ((PTR)BLKPTR((SHORT)sky_textures[panel].t)) + sizeof(BITMHDR);
	//tptr = &tptr[x*wid];
	//tptr = &tptr[x];
	tsy = (yStart*wid) + x;
	
#if !defined(_WINDOWS)
	if (fAIMoving)
	{
		do {
			if (tsy >= t_size)
			{
				//tsy -= wid;
				tsy = x;
				// textures[11] is BACK01X.PCX, which is all grass.
				// on bottom.
				tptr = ((PTR)BLKPTR((SHORT)sky_textures[11].t)) + sizeof(BITMHDR);
				//tptr = &tptr[x*wid];
			}
			*(sptr+1)=*sptr = tptr[tsy];
			sptr += sptr_inc;
			tsy += tptr_inc;
		} while (sptr < sptr_end);
	}
	else
#endif
	{
		do {
			if (tsy >= t_size)
			{
				//tsy -= t_size;
				tsy = x;
				// textures[11] is BACK01X.PCX, which is all grass.
				// on bottom.
				tptr = ((PTR)BLKPTR((SHORT)sky_textures[11].t)) + sizeof(BITMHDR);
				//tptr = &tptr[x*wid];
			}
			*sptr = tptr[tsy];
			sptr += sptr_inc;
			tsy += tptr_inc;
		} while (sptr < sptr_end);
	}
}

/* ========================================================================
   Function    - hscrape
   Description - draws the sky "ceiling"
   Returns     - void
   ======================================================================== */
void hscrape(LONG x,LONG y,LONG xe)
{
	PTR		tptr;								// pointer to sky texture
	PTR		sptr;								// pointer to background screen
	PTR		sptr_end;						// stop when we reach this point
	ULONG	wid = sky_textures[0].w;
	ULONG height = sky_textures[0].h;
	//ULONG	t_size = wid * sky_textures[0].h;
	ULONG	tsx = 0;
#if WRC_CAMERA_TEST
	ULONG	sptr_inc;
#endif
	SHORT	panel = 0;
	//ULONG	tsx_inc = wid;
	ULONG   tsx_inc = 1;
	LONG 	margin_left;
	LONG	margin_right;
	LONG	margin_top;
	LONG	margin_bottom;

	++xe;

	get_margin_size(&margin_right,&margin_left,&margin_top,&margin_bottom);

	// Calculate the screen pointer.
#if WRC_CAMERA_TEST
	if (VideoCurrentlyRedirected)
	{
		sptr = (PTR)screen+((x * screen_buffer_height) + y);
		sptr_end = (PTR)screen + ((xe * screen_buffer_height) + y);
		sptr_inc = screen_buffer_height;
	}
	else
#endif
	{
		sptr = (PTR)screen + ((y * screen_buffer_width) + x);
		sptr_end = (PTR)screen + ((y * screen_buffer_width) + xe);
#if WRC_CAMERA_TEST
		sptr_inc = 1;
#endif
	}

	// Now calculate the texture pointer by performing some exotic operations
	// on the screen x and y...

	// Handle look up and down.
	y -= (camera.p * render_height) / MAX_VIEW_HEIGHT;


#if 01
// [d10-21-96 JPC] Re-enabled this code.  (For auto-res.)
	if (!fHighRes)		// handle low res
	{
		x *= 2;
		tsx_inc *= 2;
		y *= 2;
		y -= margin_top;
	}
#endif

#if 01
	if (margin_bottom >= 164 /* BTLUI_PANEL_HEIGHT */ && margin_top == 0)
	{
		// [d11-11-96 JPC] Try to fix the battle wads so you can see the
		// mountains and so on in the background.
		// If less than full screen, just add a certain amount to y.
		// [d11-20-96 JPC] Added the margin_top test to allow for the chat
		// box that's sometimes at the top.
		y += BATTLE_WAD_ADJUST;
	}
#endif

	y += FIRST_LINE_TOP_OF_SCRAPE;	 		/* nominal first line of texture to draw as top-of-scrape */
	y = (y * (SCRAPE_Y_FACTOR<<12) ) / 100;

	x += camera.a * 15;
	panel = x >> 8;
	// [d3-19-97 JPC] Change 0xFF to (wid - 1); change 256 to height.
	// Since width and height are both 256, the following code will work
	// even if we are not using the right terms.  If the code breaks,
	// check whether the width and height are now different from 256.
	// x &= 0xff;
	x &= (wid - 1);
	// if (y<=(256<<12) && y>=0)
	if (y<=(height<<12) && y>=0)
	{
		if (panel > 9) panel-=10;
		tptr = ((PTR)BLKPTR((SHORT)sky_textures[panel].t)) + sizeof(BITMHDR);
	}
	else
	{
		tptr = ((PTR)BLKPTR((SHORT)sky_textures[10].t)) + sizeof(BITMHDR);
		// y = (y+(256<<12)) % (256<<12);
		y = (y+(height<<12)) % (height<<12);
		panel = 99;
	}

	if (!fHighRes)
		//tsx = (y>>12) + ((x-margin_left)&0xFF) * wid;
		tsx = ((y>>12)*wid) + ((x-margin_left) & (wid - 1)); // 0xFF -> (wid - 1)
	else
		//tsx = (y>>12) + (x * wid);
		tsx = ((y>>12)*wid) + x;

	// GWP I moved the panel test outside the loop for speed optimization.
	if (panel == 99)
	{
		// This paints the high sky
		if (!fHighRes)
		{
			LONG t_end = tsx + (wid - x);
			do
			{
				*sptr = tptr[tsx];
	#if WRC_CAMERA_TEST
				sptr += sptr_inc;
	#else	
				++sptr;
	#endif
				tsx += tsx_inc;
		
				if (tsx >= t_end)
				{
					//tsx -= t_size;
					tsx = (y >> 12)*wid;
					t_end = tsx + wid;
				}
			} while (sptr < sptr_end);
		}
		else
		{
			LONG texturepixLeft = wid - x;
			
			do {
				LONG pixelsLeft = 1 + (sptr_end - sptr);
				
				if (texturepixLeft < pixelsLeft)
				{
					memcpy(sptr, &tptr[tsx], texturepixLeft);
					sptr += texturepixLeft;
					
					//tsx -= t_size;
					tsx = (y>>12)*wid;
				 	texturepixLeft = wid;
				}
				else
				{
					memcpy(sptr, &tptr[tsx], pixelsLeft);
					sptr += pixelsLeft;
				}
			} while (sptr < sptr_end);
		}
	}
	else
	{
		if (!fHighRes)
		{
			LONG t_end = tsx + (wid - x);
			do {
				*sptr = tptr[tsx];
	#if WRC_CAMERA_TEST
				sptr += sptr_inc;
	#else	
				++sptr;
	#endif
				tsx += tsx_inc;
		
				if (tsx >= t_end)
				{
					//tsx -= t_size;
					tsx = (y >> 12)*wid;
					if (++panel > 9)
					{
						panel-=10;
					}
					tptr = ((PTR)BLKPTR((SHORT)sky_textures[panel].t)) + sizeof(BITMHDR);
					t_end = tsx + wid;
				}
			} while (sptr < sptr_end);
		}
		else
		{
			LONG texturepixLeft = wid - x;
			
			do {
				LONG pixelsLeft = 1 + (sptr_end - sptr);
				
				if (texturepixLeft < pixelsLeft)
				{
					memcpy(sptr, &tptr[tsx], texturepixLeft);
					sptr += texturepixLeft;
					
					//tsx -= t_size;
					tsx = (y>>12)*wid;
					if (++panel > 9)
					{
						panel -= 10;
					}
				 	tptr = ((PTR)BLKPTR((SHORT)sky_textures[panel].t)) + sizeof(BITMHDR);
				 	texturepixLeft = wid;
				}
				else
				{
					memcpy(sptr, &tptr[tsx], pixelsLeft);
					sptr += pixelsLeft;
				}
			} while (sptr < sptr_end);
		}
	}

}

/* ======================================================================== */
void WriteErrFile(char *n)
{
	FILE *f;

	// echo to screen
	printf("Texture not found:%s - Generic texture assigned\n",n);

	// write to file
	f=fopen("errfile.err","a+");
	if(f != NULL)
		fprintf(f,"Texture not found:%s - Generic texture assigned\n",n);
	fclose(f);
}

// ---------------------------------------------------------------------------
void TextureFrameHandler (LONG arg)
{
// This is the callback function for lighting effects that depend on frames.
// This is not intended to be a permanent function, but you never know.
// The function is set up in MAIN: GameMain, which calls TASK: add_task.
// ---------------------------------------------------------------------------

	++gcFrames;									// just a frame counter

	if (gcMagicTemporaryLight > 0)
	{
		--gcMagicTemporaryLight;
	}
}


/* ========================================================================
   Function    - LoadAnimatedTextureStub
   Description - Creates a dummy resource to hold current animated
					  texture frame.  (Dummy name is base texture name
					  + "ANT" extension.  The "ANT" extension is registered
					  with the resource manager in GAME.CPP: init_game.
   Returns     - Handle to memory block.
	Note        - We ignore all the parameters.  What we need is a BITM
					  that's 128 x 128 (or, for low memory, 64 x 64), so that's
					  what we create.  Code was copied from _OpenBitm, in fact.
   ======================================================================== */
SHORT LoadAnimatedTextureStub (																// )
	CSTRPTR szFileName,
	BOOL  fSetPal,
	BOOL  fLockRes,
	BOOL  fRotated,
	LONG	iResFileSlot)
{
	SHORT			iBlk;
	SHORT			scale;
	ULONG			w,h;

	// [d4-15-97 JPC] UNITARY_SCALE/2 = 2.5, which we can't track because
	// scale is an integer.  So bump up to medium res.
	w = 128;
	h = 128;
	scale = UNITARY_SCALE * 2;

	iBlk = NewBlock((w * h) + sizeof(BITMHDR));

	if (iBlk != fERROR)
	{
		BITMPTR		pBlk;
		
		pBlk = (BITMPTR)(BLKPTR(iBlk));
		pBlk->w = w;
		pBlk->h = h;
		pBlk->scale = scale;
		pBlk->x_ctr_pt = 0;
		pBlk->type = TYPEBITM;
	}

	return iBlk;
}


// ---------------------------------------------------------------------------
