/* ®RM250¯ */
/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: CONVERT.C   -Handles converting doom-stuff???
   Author: Chris Phillips
   ========================================================================
   Contains the following internal functions:

   Contains the following general functions:
   segs_match            -???
   single_sided          -Tests to see if a LineDef is single sided or not
   ssector_to_hei        -???
   seg_to_height         -???
   sector_to_fh          -floor height of a sector
   sector_to_ch          -ceiling height of a sector
   seg_to_ceil           -???
   seg_to_sector         -finds the sector a seg is in
   line_to_sector        -finds the sector a line is in
   seg_to_tname          -finds the texture name on a seg
   sector_to_light       -finds the light level of a sector
   seg_two_sided         -finds if a seg is two sided
   seg_to_texture_num    -???
   seg_to_tyoff          -finds the offset of the texture on the seg

   ======================================================================== */
/* ------------------------------------------------------------------------
   Include
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include "SYSTEM.H"
#include "ENGINE.H"
#include "ENGINT.H"
#include "LIGHT.H"
#include "SPECIAL.H"
#include "SOUND.HXX"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
	//REVISIT: Do we have all nessacary converts??
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
extern BYTE * gSectorLight;

/* =======================================================================
   Function    - segs_match
   Description - ???
   Returns     - TRUE or FALSE
   ======================================================================== */
LONG segs_match(SHORT a,SHORT b)
{
	if( (segs[a].a==segs[b].a) && (segs[a].b==segs[b].b) )
		return(TRUE);
	if( (segs[a].a==segs[b].b) && (segs[a].b==segs[b].a) )
		return(TRUE);
	return(FALSE);
}

/* =======================================================================
   Function    - single_sided
   Description - finds whether or not a line is single_sided
   Returns     - TRUE or FALSE
   ======================================================================== */
LONG single_sided(LONG s)
{
	if(linedefs[segs[s].lptr].psdt==-1)
		return(TRUE);
	return(FALSE);
}

/* =======================================================================
   Function    - ssector_to_hei
   Description - ???
   Returns     - a LONG
   ======================================================================== */
LONG ssector_to_hei(LONG s)
{
	return((LONG)seg_to_height(ssectors[s].o,0));
}

/* =======================================================================
   Function    - seg_to_height
   Description - ???
   Returns     - a LONG
   ======================================================================== */
LONG seg_to_height(LONG s,LONG side)
{
LONG l,sec,sided;
	l=(LONG)segs[s].lptr;

	if(side==1)
		sided=(LONG)linedefs[l].psdt;
	else
		sided=(LONG)linedefs[l].psdb;

	sec=(LONG)sidedefs[sided].sec_ptr;
	return((LONG)sectors[sec].fh);
}

/* =======================================================================
   Function    - sector_to_fh
   Description - finds the floor height of a sector
   Returns     - the floor height
   ======================================================================== */
LONG sector_to_fh(ULONG sec)
{
	return(sectors[sec].fh);
}

/* =======================================================================
   Function    - sector_to_ch
   Description - finds the ceiling height of a sector
   Returns     - the ceiling height
   ======================================================================== */
LONG sector_to_ch(ULONG sec)
{
	return(sectors[sec].ch);
}

/* =======================================================================
   Function    - seg_to_ceil
   Description - ???
   Returns     - a LONG
   ======================================================================== */
LONG seg_to_ceil(LONG s,LONG side)
{
LONG l,sec,sided;
	l=(LONG)segs[s].lptr;

	if(side==1)
		sided=(LONG)linedefs[l].psdt;
	else
		sided=(LONG)linedefs[l].psdb;

	sec=(LONG)sidedefs[sided].sec_ptr;
	return((LONG)sectors[sec].ch);
}

/* =======================================================================
   Function    - seg_to_sector
   Description - finds a the sector a seg is in
   Returns     - the sector
   ======================================================================== */
LONG seg_to_sector(LONG s,LONG side)
{
LONG l,sec,sided;
	l=(LONG)segs[s].lptr;

	if(side==1)
		sided=(LONG)linedefs[l].psdt;
	else
		sided=(LONG)linedefs[l].psdb;

	sec=(LONG)sidedefs[sided].sec_ptr;
	return((LONG)sec);
}

/* =======================================================================
   Function    - line_to_sector
   Description - finds the sector a line is in
   Returns     - the sector
   ======================================================================== */
LONG line_to_sector(LONG l,LONG side)
{
LONG sec,sided;

	if(side==1)
		sided=(LONG)linedefs[l].psdt;
	else
		sided=(LONG)linedefs[l].psdb;

	sec=(LONG)sidedefs[sided].sec_ptr;
	return((LONG)sec);
}

/* =======================================================================
   Function    - seg_to_tname
   Description - finds the name of the texture on a seg
   Returns     - void
   ======================================================================== */
#if 0 // UNUSED
void seg_to_tname(LONG s,LONG side,char *n)
{
LONG l,sided;
LONG i;
	l=(LONG)segs[s].lptr;

	if(side==1)
		sided=(LONG)linedefs[l].psdt;
	else
		sided=(LONG)linedefs[l].psdb;

	n[0]='-';
	n[1]=0;
	if(sided==-1)
		return;
	if(sidedefs[sided].n1[0]!='-')
		{
		for(i=0;i<9;i++)
			n[i]=sidedefs[sided].n1[i];
		n[8]=0;
		}
	else if(sidedefs[sided].n2[0]!='-')
		{
		for(i=0;i<9;i++)
			n[i]=sidedefs[sided].n2[i];
		n[8]=0;
		}
	else if(sidedefs[sided].n3[0]!='-')
		{
		for(i=0;i<9;i++)
			n[i]=sidedefs[sided].n3[i];
		n[8]=0;
		}
}
#endif

/* =======================================================================
   Function    - sector_to_light
   Description - finds the light level of the sector
   Returns     - the light level
   ======================================================================== */
LONG sector_to_light(ULONG sect)
{
	LONG			light;
	BYTE			count;
	static LONG oldgcFrames;
	static BOOL fLightning;

	light = (sectors[sect].light + 1) >> 3;
	
	// [d7-03-96 JPC] Do various things with light depending on "special" value of sector.
	switch (sectors[sect].special)
	{
		case SSP_LIGHTNING:
			// Every sector marked as a lightning sector lights up if
			// there's lighting.  Do the random test for lightning once
			// per frame.
			if (oldgcFrames != gcFrames)
			{
				oldgcFrames = gcFrames;
				// Change the random argument: higher for less frequent lightning,
				// lower for more frequent lightning.
				if (random (20) == 0)
				{
					fLightning = TRUE;
					// [d11-01-96 JPC] Add some thunder.
					// [d11-10-96 JPC] Change VOLUME_FULL to VOLUME_SIXTY.
					AddSndObj (SND_THUNDER1, SND_THUNDER_TOTAL, VOLUME_SIXTY);
				}
				else
				{
					fLightning = FALSE;
				}
			}
			if (fLightning)
				return 0;						// premature return, brightest light
			break;

      case SSP_BLINK:
	      // blink every second (actually 10 frames)
			// if ((gcFrames % 20) < 10)
			// 	light = 0;
			// Change: randomly flicker every frame.  Did not work because
			// EVERY span calls this routine (which we should look into
			// for possible speedup).
			// light -= random (10);
			count = (BYTE) (gcFrames & 0x07);
			if ((gSectorLight[sect] & 0x7) == count || gSectorLight[sect] == 0)
			{
				// Light level is in upper 5 bits.
				light += random (3);
				if (light > 31)
					light = 31;
				// Set new random counter in low 3 bits.
				count += random(2) + 1;		// 1 or 2
				count &= 0x07;
				gSectorLight[sect] = (light << 3) + count;
			}
			else
			{
				light = gSectorLight[sect] >> 3;
				// ASSERT (light < 32);
			}
			break;

	}
	// Now convert to Birthright scale where 0 = brightest, 31 = darkest.
	// (This function can actually return 32, which is too many, but callers
	// fix it.  Should this be changed?  Probably not--too delicate.)
	return 32 - light;
}


/* =======================================================================
   Function    - seg_two_sided
   Description - finds if a seg is two sided
   Returns     - TRUE or FALSE
   ======================================================================== */
LONG seg_two_sided(LONG s)
{
	if(linedefs[segs[s].lptr].psdt==-1)
		return(FALSE);
	return(TRUE); /*assume true*/
}

/* =======================================================================
   Function    - seg_to_texture_num
   Description - ???
   Returns     - a LONG
   ======================================================================== */
LONG seg_to_texture_num(LONG seg,LONG side,LONG pos)
{
// [d6-05-96 JPC] Swapped LOWER_TEXTURE return and MIDDLE_TEXTURE return
// to conform to actual meanings of n2 and n3.  This means that when
// we want the middle texture, we can ask for MIDDLE_TEXTURE instead of
// LOWER_TEXTURE and vice versa.
// ---------------------------------------------------------------------------

   LONG l,sided;

	l=(LONG)segs[seg].lptr;

	if(side==1)
		sided=(LONG)linedefs[l].psdt;
	else
		sided=(LONG)linedefs[l].psdb;

	if(sided==-1)
		return(0);
	switch(pos)
		{
		case LOWER_TEXTURE:
			return((ULONG)sidedefs[sided].n2); // was n3
			//break;
		case MIDDLE_TEXTURE:
			return((ULONG)sidedefs[sided].n3); // was n2
			//break;
		case UPPER_TEXTURE:
			return((ULONG)sidedefs[sided].n1);
			//break;
		}
	return(0);
}

/* =======================================================================
   Function    - seg_to_txoff_tyoff
   Description - finds the x-offset of the texture on a side of the seg
   Returns     - the offset
   Comment     - Combined the x & y retrival for speed. -GWP-
   ======================================================================== */
void seg_to_txoff_tyoff(LONG seg,LONG side, LONG * ptxoff, LONG * ptyoff)
{
   LONG l,sided;

	l=(LONG)segs[seg].lptr;

	if(side==1)
		sided=(LONG)linedefs[l].psdt;
	else
		sided=(LONG)linedefs[l].psdb;

	if(sided==-1)
	{
		*ptxoff = 0;
		*ptyoff = 0;
	}
	else
	{
		*ptxoff = sidedefs[sided].xoff;
		*ptyoff = sidedefs[sided].yoff;
	}
}
/* =======================================================================
   Function    - seg_to_txoff
   Description - finds the x-offset of the texture on a side of the seg
   Returns     - the offset
   Comment     - JPC added this 6-07-96.  Copied from seg_to_tyoff.
                 Called from draw_wall, etc., in RENDER.C.
   ======================================================================== */
LONG seg_to_txoff(LONG seg,LONG side)
{
   LONG l,sided;

	l=(LONG)segs[seg].lptr;

	if(side==1)
		sided=(LONG)linedefs[l].psdt;
	else
		sided=(LONG)linedefs[l].psdb;

	if(sided==-1)
		return(0);
	return(sidedefs[sided].xoff);
}


/* =======================================================================
   Function    - seg_to_tyoff
   Description - finds the y-offset of the texture on a side of the seg
   Returns     - the offset
   ======================================================================== */
LONG seg_to_tyoff(LONG seg,LONG side)
{
LONG l,sided;

	l=(LONG)segs[seg].lptr;

	if(side==1)
		sided=(LONG)linedefs[l].psdt;
	else
		sided=(LONG)linedefs[l].psdb;

	if(sided==-1)
		return(0);
	return(sidedefs[sided].yoff);
}

