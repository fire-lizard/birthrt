/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: NEWSYS.C
   Author: Alan Clark
   ========================================================================
   Contains the following internal functions:
   Exists					- Tests to see if a file exists in the current dir
   text_len					- finds the length of a string

   Contains the following general functions:
	SaveVideoSet			-
	RestoreVideoSet		-
   set_view_size			- sets the view size & position
   set_margin_size		-
   set_screen_size		-
	plot						-
	get_pixel				-
	line						-
	detect_scale			-

   gplot_char				- plots a character
   gtext_width				- calculates the width of a string
   gtext_height			- calculates the height of a string
   gprint_text				- prints a line
  	print_text_centered	- centers and prints a line
   print_textf				- allows formatting in gprint_text
   init_gfont				- load a font

	======================================================================== */

/*	------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#include <ctype.h>
#include <stdarg.h>
#include "typedefs.h"
#include "system.h"
#include "engine.h"

#define INTER_CHAR_SPACE	1
#define NORM_SCALE			(1<<8)

extern SHORT		xCharWidth[256 + 32];
extern SHORT		xCharPosn[256 + 32];
extern SHORT		iCurFont;
extern SHORT		wCurFont;
extern SHORT		hCurFont;
extern SHORT		yFontBase;
extern LONG		fBold;
extern BOOL		fItalic;
extern BOOL		fNonRemapped;
extern LONG		wrap_width;
extern LONG		wrap_height;
extern LONG		tab_width;
extern LONG		scale;
extern LONG		inter_char_space;
extern char		cFraction[];
extern LONG		last_print_offset;
extern LONG		desired_context_number;

extern UBYTE antia_table[256 * 256];
extern UBYTE lighten[256];

extern ULONG ConvertWinForeignToASCII(ULONG win_char);
extern void ZoomOut(LONG /* type */, LONG);
extern void MoveZoomMap(LONG xx, LONG yy);

/* ------------------------------------------------------------------------
   Defines
   ------------------------------------------------------------------------ */
#define	LEFT_JUSTIFY		1
#define	RIGHT_JUSTIFY		2
#define	CENTER_JUSTIFY		3
#define	DO_WRAP				4

/* ------------------------------------------------------------------------
	prototypes
   ------------------------------------------------------------------------ */

/* =======================================================================
   Function    - 
   Description - 
   Returns     - 
   ======================================================================== */
LONG GetNumber (char *text, LONG * ii)
{
	LONG	rv = 0;
	LONG	i = ii[0];

	while (isdigit(text[i]))
	{
		rv *= 10;
		rv += (text[i++]-'0');
		ii[0]++;
	}
	//printf("GetNumber:%ld",rv);
	return rv;
}

CSTRPTR GetFilename (char *text, LONG * ii)
{
	static char buff[60];
	LONG	i = ii[0];
	LONG	cnt = 0;

											//  ` and | = non-printing terminators
	while (cnt < 59 && text[i] != '`' && text[i] != '|' && text[i] != ',')
	{
		buff[cnt++] = text[i++];
		buff[cnt] = 0;
		ii[0]++;
		if (cnt > 4 && buff[cnt-4] == '.')
		{
			//printf("GetFilename:%s",buff);
			return &buff[0];
		}
	}
	//printf("GetFilename:%s",buff);
	return &buff[0];
}

/* =======================================================================
   Function    - init_gfont
   Description - loads the alt font
   Returns     - void
   ======================================================================== */
void init_gfont_(CSTRPTR filename)
{
	LONG		  	x;
	LONG		  	xO;
	LONG		  	y;
	//LONG			yO;
	LONG			ascii_val;
	SHORT			iFont;
	PTR			p;
	char			n[256 + 32];

	sprintf(n, "%s%s", FONT_PATH, filename);
	iFont = GetResourceRotNoScale(n);				// load .PCX file for font

	if (iFont == fERROR || iFont == iCurFont)		// don't process same font again
		return;

	if (iCurFont != fERROR)
		SetPurge(iCurFont);	// purge old font

	iCurFont = iFont;
	wCurFont = ((BITMPTR)BLKPTR(iCurFont))->h;
	hCurFont = ((BITMPTR)BLKPTR(iCurFont))->w;

	p = ((PTR)BLKPTR(iCurFont)) + sizeof(BITMHDR);

	for (y = 0; y < hCurFont; ++y)				/* scan for font base */
		if (p[y])								/* check pixel */
			yFontBase = y;

	for (ascii_val = xO = x = 0; x < wCurFont && ascii_val < (256 + 32); ++x)	/* scan for character positions */
	{
		if (p[x * hCurFont])					/* check pixel */
		{
			if (ascii_val)
				xCharWidth[ascii_val - 1] = (SHORT)(x - xO);
			xCharPosn[ascii_val] = (SHORT)x;

			xO = x;
			++ascii_val;
		}
	}

}

/* =======================================================================
   Function    - gtext_width_sub
   Description - finds the width of a string in the alt font
   Returns     - the width
   ======================================================================== */
LONG gtext_width_sub__ (char *txt, LONG Mode)
{
	char * text = txt;
	LONG i, width, max_wid, c;
	LONG lastspace = -1;
	LONG fBoldLocal = fBold;
	SHORT	fDecimal = FALSE;

	if(text == NULL)
		return 0;

	//printf("Width - Mode:%ld Text:%s\n",Mode,txt);

	max_wid = width = i = 0;

	while ((c=text[i++]))
	{
		if (c == ' ') lastspace = i-1;

		if (c == '^')											/* handle commands */
		{
			c = text[i++];
			switch (c)
			{
				case 'f':								/* change font */
					init_gfont_(GetFilename(text, &i));
					break;
				case 's':								/* inter-char space */
					inter_char_space = GetNumber(text, &i);
					break;
				case 'b':								/* bold  */
					fBoldLocal = !fBoldLocal;
					break;
				case 'd':								/* decimal */
					fDecimal = TRUE;
					break;
				case 'z':								/* zoom */
					if (text[i] == 'o')
						i++;
					else
					{
						GetNumber(text, &i);
						i++;
						GetNumber(text, &i);
					}
					break;
				case 'c':								/* 2nd char and number */
				case 'x':
				case 'y':
					i++;
				case 'h':								/* number */
				case 'r':
				case 't':
				case 'w':
					GetNumber(text, &i);
					break;
				case 'j':								/* 2nd char */
					i++;
					break;
				case 'i':								/* single char */
				case 'n':
					break;
			}
		}

		else if (c=='`')								// non-printing terminator
		{
		}
		else if (c=='|')								// non-printing width terminator
		{
			return (width);
		}

		else if ((c-32) >= 0)						/* printable characters */
		{
			if (c > 127)
			{
				c = ConvertWinForeignToASCII ( c-128 );
				c += 32;
			}

			// handle fractional numbers
			if (fDecimal == TRUE && c>='0' && c<='9')
			{
				++fDecimal;
			}
			if (c!='0' && fDecimal == TRUE+1 && (text[i]<'0' || text[i]>'9'))
			{
				fDecimal = FALSE;
				c = cFraction[c-'0'];
			}

			width = width + (((xCharWidth[c-32] + inter_char_space + fBoldLocal) * scale) / NORM_SCALE);
		}
		else if (c==13)								// carriage-return
		{
		}
		else if (c==10)								// linefeed (newline)
		{
			if (Mode == RIGHT_JUSTIFY || Mode == CENTER_JUSTIFY)
			{
				//printf("Width J:%ld\n,",width);
				return (width);
			}
			if (width > max_wid)
				max_wid = width;
			lastspace = -1;		// shouldn't wrap back across \n boundary
			width = 0;
		}
		else if (c == 9)
		{
			SHORT const tabstop = width / tab_width;
			width = (tabstop + 1) * tab_width;
			lastspace = i-1;	// tabs are whitespace
		}

		// special code to suborn this function for word wrap
		if (Mode==DO_WRAP && wrap_width > 0 && width > wrap_width && lastspace > 0)
		{
			text[lastspace] = 10;			// change to return (\n)
			
			// GWP This prevents the next line from starting with a space
			// It doesn't really test for \r, just that its a non printable 
			// character.
			if (text[lastspace+1] == ' ')	// two spaces after punctuation
			    text[++lastspace] = '\r';	// a no-op after return
			    
			return (lastspace+1);
		}
	}

	if (width > max_wid)
		max_wid = width;

	if (wrap_width > 0)		// wordwrap terminates with 0
		return 0;

	return max_wid;
}


LONG gtext_width__(char *text)
{
	inter_char_space = INTER_CHAR_SPACE;
	return gtext_width_sub__(text,FALSE);
}

/* =======================================================================
   Function    - gtext_height
   Description - finds the height of a string in the alt font
   Returns     - the height
   ======================================================================== */
LONG gtext_height_(char *txt)
{
	LONG i, height, c;
	char * text = txt;

	if(text == NULL)
		return 0;
	i = 0;
	wrap_height = -1;
	height = hCurFont;
	while ((c=text[i++]))
	{
		if (c==10)  /* handle return (\n) */
		{
			height += hCurFont;
			if (wrap_height != -1 && height + hCurFont > wrap_height)
			{
				last_print_offset = i;		// store value away
				return height;
			}
		}

		if (c=='^')
		{
			c = text[i++];

			if (c == 'f')								/* change font */
			{
				init_gfont_(GetFilename(text, &i));
			}

			if (c == 'w')								/* word wrapped */
			{
				LONG ii;
				wrap_width = GetNumber(text, &i);

				for (ii=i; ii != 0; ii = gtext_width_sub__(&text[ii],DO_WRAP))
				{
				}
				//ii = i;
				//while ( (ii = gtext_width_sub(&text[ii])) )  // wrap is built into gtext_width
				//	ii = ii;		// dummy
				wrap_width = -1;		// turn it off
			}
			
			//if (c == 'H')								/* height limited */
			//{
			//	wrap_height = GetNumber(text, &i);
			//}
		}
	}
	return height;
}

/* =======================================================================
   Function    - gprint_text
   Description - print a line of text in the alternate font
   Returns     -
   ======================================================================== */
SHORT gprint_text_ (LONG x, LONG y, char *text, LONG color)
{
	LONG	i = 0;
	ULONG	c;
	LONG	margin_x, start_x, start_y, angle_x, angle_;
	LONG	ascale_x, h_ascale_x, ascale_y, h_ascale_y;
	LONG	yb = 9999;
	LONG	le = 9999;
	LONG	max_h = 0;
	LONG	antialias = color * 256;
	LONG	antialias2 = color * 256;
	LONG	OrigColor = color;
	SHORT	yHeight = 0;
	BOOL	fAngle = FALSE;
	BOOL	fTranslucent = FALSE;
	BOOL	fUnderline = FALSE;
	BOOL	fJustify = LEFT_JUSTIFY;
	SHORT	fDecimal = FALSE;
	SHORT	iBitm = fERROR;
	SHORT	iBkgnd= fERROR;
	PTR	pfont;

	if (iCurFont < 1)
		return 0;

	if(text == NULL)
		return 0;

	pfont = ((PTR)BLKPTR(iCurFont)) + sizeof(BITMHDR);
	if (!IsPointerGood(pfont))
		return 0;

	fBold = FALSE;
	fItalic = FALSE;
	fNonRemapped = FALSE;
	wrap_width = -1;
	wrap_height = -1;
	ascale_x = ascale_y = scale = NORM_SCALE;
	h_ascale_x = h_ascale_y = (NORM_SCALE * 2) / 3;
	inter_char_space = INTER_CHAR_SPACE;
	margin_x = x;
	start_x = x;
	start_y = y;

	while ((c = (unsigned char)text[i++]))
	{
		if (c == 10)						/* handle linefeed (\n) */
		{
			if (fUnderline) line(le,yb+1,x,yb+1,color);
			x = margin_x;
			//printf("Margin X:%ld\n",margin_x);
			y += max_h+1;
			yb += max_h+1;
			yHeight += max_h+1;
			if (!text[i]) 					/* if nothing after return */
				return yHeight+max_h+1;
			if (wrap_height != -1 && yHeight + max_h * 2 > wrap_height)
			{
				last_print_offset = i;		// store value away
				return yHeight+max_h+1;
			}
			max_h = 0;
			if (fJustify == RIGHT_JUSTIFY)
			{
				x -= gtext_width_sub__(&text[i],RIGHT_JUSTIFY);
				//printf("Right Justify X:%ld\n",x);
			}
			else if (fJustify == CENTER_JUSTIFY)
			{
				x -= (gtext_width_sub__(&text[i],CENTER_JUSTIFY)/2);
				//printf("Center Justify X:%ld",x);
			}
			continue;
		}
		else if (c == 13)						// carriage-return
		{
			// assume linefeed comes with carriage-return
			continue;
		}
		else if (c == 9)							/* handle tab (\t) */
		{
			SHORT tabstop;
			tabstop = (x - margin_x) / tab_width;
			if (tabstop >= 0)	// don't mess with unlikely values
				x = margin_x + (tabstop + 1) * tab_width;
			continue;
		}
		else if (c == '`' || c == '|')					// non printing terminators
		{
			continue;
		}
		else if (c == '^')									/* handle commands */
		{
			c = text[i++];
			switch (c)
			{
				//case 'a':									/*  */
				//	break;
				case 'b':									/* bold  */
					fBold = !fBold;
					break;
				case 'c':									/* set colors */
					if (text[i] == 'a')					/* change antialias color */
					{
						i++;
						antialias2 = GetNumber(text, &i) * 256;
					}
					else if (text[i] == 'n')			/* non color remapped */
					{
						i++;
						fNonRemapped = !fNonRemapped;
					}
					else										/* change foreground color */
					{
						color = GetNumber(text, &i);
						if (color == 0)	// trap color zero to return to orig color
							color = OrigColor;
						antialias = color * 256;
					}
					break;
				case 'd':									/* print number as decimal */
					fDecimal = TRUE;
					break;
				case 'f':									/* change font */
					init_gfont_(GetFilename(text, &i));
					pfont = ((PTR)BLKPTR(iCurFont)) + sizeof(BITMHDR);
					if (yb != 9999)
						y = yb - yFontBase;
					break;
				case 'h':									/* context sensitive help */
					{
						LONG j = GetNumber(text, &i);
						if (desired_context_number == j)		// how does this work?
							desired_context_number = 0;
					}
					break;
				case 'i':									/* italic  */
					fItalic = !fItalic;
					break;
				case 'j':									/* justify */
					c = text[i++];
					if (c == 'r')							/* justify right */
					{
						x -= gtext_width_sub__(&text[i],RIGHT_JUSTIFY);
						fJustify = RIGHT_JUSTIFY;
					}
					else if (c == 'c')					/* justify center */
					{
						x -= (gtext_width_sub__(&text[i],CENTER_JUSTIFY)/2);
						fJustify = CENTER_JUSTIFY;
					}
					else if (c == 'l')					/* justify left */
						fJustify = LEFT_JUSTIFY;
					break;
				case 'n':									/* new page */
					if (wrap_height != -1)  // only useful with ^H???
						yHeight = wrap_height;
					break;
				case 'r':									/* rotate text to angle */
					fAngle = !fAngle;
					angle_x = x;
					angle_ = GetNumber(text, &i);
					break;
				case 's':								/* inter-char space */
					inter_char_space = GetNumber(text, &i);
					break;
				case 't':									/* print translucent */
					fTranslucent = GetNumber(text, &i);
					break;
				case 'u':									/* print underline */
					fUnderline = !fUnderline;
					if (fUnderline==FALSE)	// just turned off
						line(le,yb+1,x,yb+1,color);
					break;
				case 'w':									/* word wrapped */
					{
						LONG ii, temp;
						wrap_width = GetNumber(text, &i);
						ii = i;
						while (temp = gtext_width_sub__(&text[ii],DO_WRAP))
							ii += temp;
						wrap_width = -1;		// turn it off
					}
					break;
				case 'x':
					if (text[i] == 'c')					/* x center */
						{i++; x -= gtext_width_sub__(&text[i],FALSE)/2;}
					else if (text[i] == 'm')			/* x margin */
						{i++; x = margin_x = start_x + GetNumber(text, &i);}
					else if (text[i] == 's')			/* x scale */
					{
						LONG xs;
						i++;
						xs = GetNumber(text, &i);
						if (xs==0) xs = 1;
						scale = (NORM_SCALE * xs) / 100;		// calc scale
						ascale_x = (NORM_SCALE * 100) / xs;
						h_ascale_x = (ascale_x * 2) / 3;
					}
					else										/* x position */
						x = start_x + GetNumber(text, &i);
					break;
				case 'y':
					if (text[i] == 'c')					/* y center */
						{i++; y -= (hCurFont*scale)/(NORM_SCALE*2);}
					else if (text[i] == 's')			/* y scale */
					{
						LONG ys;
						i++;
						ys = GetNumber(text, &i);
						if (ys==0) ys = 1;
						scale = (NORM_SCALE * ys) / 100;		// calc scale
						ascale_y = (NORM_SCALE * 100) / ys;
						h_ascale_y = (ascale_x * 2) / 3;
					}
					else										/* y position */
						y = start_y + GetNumber(text, &i);
					break;
				case 'z':
					if (text[i] == 'o')					/* zoom out */
						{i++; ZoomOut(0,0);}
					else										/* zoom in and/or move map */
					{
						LONG xx, yy;
						++i;	// i
						xx = GetNumber(text, &i);
						++i;	// comma
						yy = GetNumber(text, &i);
						MoveZoomMap(xx,yy);
					}
					break;

				case 'B':									/* button */
					{
						LONG xx, yy, w, h, key, func, val1, val2, fDel, fState;
						xx = GetNumber(text, &i); i++;
						yy = GetNumber(text, &i); i++;
						if (xx) xx += start_x; else xx = x;
						if (yy) yy += start_y; else yy = y;
						w = GetNumber(text, &i); i++;
						h = GetNumber(text, &i); i++;
						key = text[i++]; i++;
						if (key=='`') key = 0;
						func = GetNumber(text, &i); i++;
						val1 = GetNumber(text, &i); i++;
						val2 = GetNumber(text, &i); i++;
						fDel = GetNumber(text, &i); i++;
						fState = GetNumber(text, &i);
						if (fDel) del_region((PFVLL)func, key);
						DrawLittleButton(xx,yy,w,h,fState);
						if (func != 0)
							add_region(xx, yy, w, h, key, (PFVLL)func, val1, val2, 0, -1);
						x = xx + (w/2);
						x -= gtext_width_sub__(&text[i],FALSE)/2;
						if (!fState) color += 2; else color = OrigColor;
					}
					break;

				case 'C':									/* draw crease */
					{
						LONG xx, yy, w, h;
						xx = start_x + GetNumber(text, &i); i++;
						yy = start_y + GetNumber(text, &i); i++;
						w = GetNumber(text, &i); i++;
						h = GetNumber(text, &i);
						crease(xx, yy, w, h);
					}
					break;

				case 'G':									/* draw graphic */
					{
						LONG xx, yy;
						SHORT iTempRes = GetResourceStd(GetFilename(text, &i), FALSE);
						i++;
						xx = GetNumber(text, &i); i++;
						if (xx) xx += start_x; else xx = x;
						yy = GetNumber(text, &i);
						if (yy) yy += start_y; else yy = y;
						if (iTempRes!=fERROR)
						{
							DrawBitmap(xx, yy, iTempRes, 0, 0, 999, 999);
							SetPurge(iTempRes);
						}
					}
					break;

#if 0
				case 'F':									/* fill rect */
					if (text[i] == 'c')					/* colored edged rect */
					{
						color_edged_rect(x,y,w,h,color);
					}
					if (text[i] == 's')					/* shaded edged rect */
					{
						shade_edged_rect(x, y, w, h, 33);
					}
					break;

				case 'R':
					del_region(func, key);
					add_region(x, y, w, h, key, func, val1, val2, id, idToolTip);
					break;
#endif

#if 0
			else if (c == 'H')								/* height limited */
			{
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]) ||
					!isdigit(text[i+2]))
				{
					return 0;
				}
				wrap_height = (text[i++]-'0') * 100;
				wrap_height += (text[i++]-'0') * 10;
				wrap_height += (text[i++]-'0');
			}
			else if (c == 'Y')								/* decrease Y coord */
			{
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]))
				{
#if defined(_DEBUG)
					fatal_error("ERROR: Bad decrease y coord. specfication %s\n", text);
#else
					return 0;
#endif
				}
				y -= (text[i++]-'0') * 10;
				y -= text[i++]-'0';
			}
			else if (c == 'S')								/* scale */
			{
				LONG j;
				LONG k;
				LONG l;
				
				if (!isdigit(text[i]) ||
					!isdigit(text[i+1]) ||
					!isdigit(text[i+2]))
				{
#if defined(_DEBUG)
					fatal_error("ERROR: Bad scale specfication %s\n", text);
#else
					return 0;
#endif
				}
				j = (text[i++]-'0') * 100;
				j += (text[i++]-'0') * 10;
				j += (text[i++]-'0');
				if (j==0) j = 1;
				k = font_info[cur_type_and_size].size_points * j;	// desired pt size x100
				l = (inter_char_space * 100) / font_info[cur_type_and_size].size_points;
TRY_SMALLER_FONT:
				scale = (NORM_SCALE * j) / 100;						// calc scale
				if (j < 100)	// scale %
				{
					LONG s = font_info[cur_type_and_size].next_smaller;	// next smaller font
					if (s >= 0 && (font_info[s].size_points*100) >= k) // -1 means none smaller
					{
						inter_char_space = (l * font_info[s].size_points) / 100;
						if (inter_char_space < 0) inter_char_space = 0;
						j = (k + (font_info[s].size_points/2)) / font_info[s].size_points;			// recalc %
						if (s==0) j = 100;								// minimum size
						init_gfont_(s);
						pfont = ((PTR)BLKPTR(iCurFont)) + sizeof(BITMHDR);
						if (yb != 9999)
							y = yb - yFontBase;
						goto TRY_SMALLER_FONT;
					}
					if (cur_type_and_size != FONT_TIMS_30PT)	// make full size
						j = 100;
				}
				ascale_x = ascale_y = (NORM_SCALE * 100) / j;
				h_ascale_x = h_ascale_y = (ascale_x * 2) / 3;
#endif
			}

			continue;
		}

		// needed to print European characters
		if (c>127)
		{
			c = ConvertWinForeignToASCII(c-128);	// extended characters
			c += 32;											// offset in our fonts
		}

		// needed to print magic characters
		if (c<32) c+=128;									// extended characters

		if ((c>=32) && (c < (256+32)))						// printable character
		{
			LONG xe;
			LONG xh;
			PTR	fptr;
			LONG ye;

			// handle fractional numbers
			if (fDecimal == TRUE && c>='0' && c<='9')
			{
				++fDecimal;
			}
			if (c!='0' && fDecimal == TRUE+1 && (text[i]<'0' || text[i]>'9'))
			{
				fDecimal = FALSE;
				c = cFraction[c-'0'];
			}

			c = c - 32;
			if (x < le) le = x;
			xe = x + xCharWidth[c];							/* x end */
			xh = x + (xCharWidth[c]/2);
			ye = y + hCurFont - 1;							/* y end */
			if (yb == 9999)
				yb = y + yFontBase;
			if (max_h < (hCurFont-1))
				max_h = (hCurFont-1);

			/* set font pointer to start of char */
			fptr = pfont + ((LONG)xCharPosn[c] * (LONG)hCurFont) + 1L;	/* add start within font to ptr */

			if (scale == NORM_SCALE)
			{
				LONG xx;
				
				/* clip at region edge */
				if ( (xe>screen_buffer_width-1) || (xe>clip_x-1) || x<0 || x<origin_x ||
						(ye>screen_buffer_height-1) || (ye>clip_y-1) || y<0 || y<origin_y )
					goto skip;

				/* loop for x */
				for (xx=x; xx<xe; ++xx, ++fptr)
				{

					// print text at an angle
					if (fAngle)
					{
						LONG yy;
						PTR sptr = screen + ( (y+((xx-angle_x)/2)) * screen_buffer_width) + xx;		/* set screen pointer start */
						/* loop for y */
						for (yy=y; yy<ye; ++yy, ++fptr, sptr+=screen_buffer_width)
						{
							LONG pix = *fptr;
							
							if (pix)							/* check for transparent */
							{
								if (pix==31 && (yy&1)==0)
									*sptr = (char)color;
								else
									*sptr = antia_table[antialias+*sptr];
							}
							if (yy&1)
							{
								--sptr;
								if (pix)
									*sptr = antia_table[antialias+*sptr];
							}
						}
					}

					// print text translucent
					else if (fTranslucent)
					{
						LONG yy;
						PTR sptr = screen + (y * screen_buffer_width) + xx;		/* set screen pointer start */
						
						/* loop for y */
						for (yy=y; yy<ye; ++yy, ++fptr, sptr+=screen_buffer_width)
						{
							LONG pix = *fptr;
							
							if (pix)			/* check for transparent */
							{
								if (fTranslucent==1)
								{
									if (pix == 31)
										*sptr = antia_table[antialias+antia_table[antialias+*sptr]];
									else if (pix==8)
										*sptr = lighten[*sptr];
									else
										*sptr = antia_table[antialias+*sptr];
								}
								else if (fTranslucent==2)
								{
									if (pix == 31)
										*sptr = antia_table[antialias+*sptr];
									else if (pix==8)
										*sptr = lighten[*sptr];
									else
										*sptr = antia_table[(antia_table[antialias+*sptr]*256) + *sptr];
								}
								else if (fTranslucent==3 && pix!=8)
								{
									*sptr = antia_table[(antia_table[antialias+*sptr]*256) + *sptr];
								}
							}
						}
					}

					// print text bold
					else if (fBold)
					{
						LONG yy;
						PTR sptr = screen + (y * screen_buffer_width) + xx;		/* set screen pointer start */
						
						/* loop for y */
						for (yy=y; yy<ye; ++yy, ++fptr, sptr+=screen_buffer_width)
						{
							LONG pix = *fptr;							/* check for transparent */
							if (pix)							/* check for transparent */
							{
								if (pix==31)
									*sptr = *(sptr+1) = (char)color;
								else
								{
									if (xx<xh)
										*sptr = antia_table[antialias+*sptr];
									else
										*(sptr+1) = antia_table[antialias+*(sptr+1)];
								}
							}
						}
					}

					// print text non-color remapped
					else if (fNonRemapped)
					{
						LONG yy;
						PTR sptr = screen + (y * screen_buffer_width) + xx;		/* set screen pointer start */
						/* loop for y */
						for (yy=y; yy<ye; ++yy, ++fptr, sptr+=screen_buffer_width)
						{
							LONG pix = *fptr;
							if (pix)							/* check for transparent */
								*sptr = pix;
						}
					}

					// print text normal
					else
					{
						LONG yy;
						PTR sptr = screen + (y * screen_buffer_width) + xx;		/* set screen pointer start */
						/* loop for y */
						for (yy=y; yy<ye; ++yy, ++fptr, sptr+=screen_buffer_width)
						{
							LONG pix = *fptr;							/* check for transparent */
							if (pix)							/* check for transparent */
							{
								if (pix==31)
									*sptr = (char)color;
								else if (pix==8)
									//*sptr = antia_table[(antia_table[antialias2+*sptr]*256)+*sptr];
									*sptr = lighten[*sptr];
								else
									*sptr = antia_table[antialias+*sptr];
							}
						}
					}

				}		// loop for xx
			}

			/* --------------------- */
			/* draw scaled           */
			/* --------------------- */
			else
			{
				LONG xs;
				LONG xx;
				
				xe = x + (((xCharWidth[c]*NORM_SCALE)+h_ascale_x)/ascale_x);		/* x end */
				ye = y + ((((hCurFont-1)*NORM_SCALE)+h_ascale_y)/ascale_y);		/* y end */

				/* clip at region edge */
				if ( (xe>screen_buffer_width-1) || (xe>clip_x-1) || x<0 || x<origin_x ||
						(ye>screen_buffer_height-1) || (ye>clip_y-1) || y<0 || y<origin_y )
					goto skip;

				/* loop for x */
				for (xx=x,xs=0; xx<xe; ++xx, xs+=ascale_x)
				{
					LONG yy;
					LONG ys;
					PTR sptr = screen + (y * screen_buffer_width) + xx;		/* set screen pointer start */
					PTR fptr = pfont + ( ((LONG)xCharPosn[c]+((LONG)xs>>8)) * (LONG)hCurFont) + 1L;

					/* loop for y */
					for (yy=y,ys=0; yy<ye; ++yy, ys+=ascale_y, sptr+=screen_buffer_width)
					{
						LONG pix;
						
						if (scale > (NORM_SCALE/2))
							pix = fptr[ys>>8];
						else
						{
							LONG yt = ys>>8;
//							pix = av[fptr[yt]] + av[fptr[yt+hCurFont]] + av[fptr[yt+1]] + av[fptr[yt+hCurFont+1]];
//							if (pix < 3) pix = 0;
//							else if (pix > 5) pix = 31;
							pix = fptr[yt] + fptr[yt+hCurFont] + fptr[yt+1] + fptr[yt+hCurFont+1];
							if (pix < 48) pix = 0;
							else if (pix > 61) pix = 31;
						}

						if (pix)				/* check for transparent */
						{
							if (fTranslucent)
							{
								if (fTranslucent==1)
								{
									if (pix == 31)
										*sptr = antia_table[antialias+antia_table[antialias+*sptr]];
									else if (pix==8)
										*sptr = lighten[*sptr];
									else
										*sptr = antia_table[antialias+*sptr];
								}
								else if (fTranslucent==2)
								{
									if (pix == 31)
										*sptr = antia_table[antialias+*sptr];
									else if (pix==8)
										*sptr = lighten[*sptr];
									else
										*sptr = antia_table[(antia_table[antialias+*sptr]*256) + *sptr];
								}
								else if (fTranslucent==3 && pix!=8)
									*sptr = antia_table[(antia_table[antialias+*sptr]*256) + *sptr];
							}
							else if (fNonRemapped)
							{
								*sptr = pix;
							}
							else
							{
								if (pix==31)
									*sptr = (char)color;
								else if (pix==8)
									//*sptr = antia_table[(antia_table[antialias2+*sptr]*256)+*sptr];
									*sptr = lighten[*sptr];
								else
									*sptr = antia_table[antialias+*sptr];
							}
						}
					}
				}
			}

skip:
			/* advance to next avail x pos */
			x += (((xCharWidth[c] + inter_char_space + fBold) * scale)+(NORM_SCALE/2))/NORM_SCALE;
		}
	}

	if (fUnderline) line(le,yb+1,x,yb+1,color);

	return yHeight+max_h+1;
}

/* =======================================================================
   Function    - print_text_centered
   Description - centers and prints a line of text in the alt font
   Returns     - void
   ======================================================================== */
#if 0
void print_text_centered(LONG x,LONG y, char * text,LONG color)
{
	if (text == NULL)
		return;
	print_textf(x-(gtext_width(text)/2),y-(hCurFont/2),color,text);
}
#endif
/* =======================================================================
   Function    - print_textf
   Description - prints a formatted line of text in the alt font
   Returns     - void
   ======================================================================== */
SHORT print_textf_(LONG x, LONG y, LONG color, const char *format, ...)
{
	char texbuffer[2000];
	va_list argp;

	LONG	i = (LONG) format;

	if ( i <= 0 )				// prevent NULL or -1 strings from crashing
		return 0;

	va_start(argp, format);
	vsprintf(texbuffer,format,argp);
	va_end(argp);
	return gprint_text_(x, y, texbuffer, color);
}

/* ======================================================================== */


