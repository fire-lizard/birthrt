/* ========================================================================
   SDLSYS.CPP - the SDL2 host backend for Linux/macOS.

   Replaces WINSYS/MACHINE.C, SYSGRAPH.CPP, MOUSE.C, KEYINT.C and UTIL.C.
   Those five files are the whole of the game's contract with the host OS
   (MACHINE.H declares it); everything above them is portable.

   The game still renders into a 640x480 8-bit paletted buffer exactly as
   it did in 1997.  The only difference here is what happens to that
   buffer at present time: instead of a GDI DIB section blitted through
   StretchBlt, it is expanded through a palette LUT into a streaming
   texture and scaled by the renderer.
   ======================================================================== */
#include "../PLATFORM.H"

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../TYPEDEFS.H"
#include "../SYSTEM.H"
#include "../MACHINE.H"
#include "../MACHINT.H"
#include "../ENGINE.H"
#include "../MAIN.HXX"
#include "../FILEUTIL.H"
#include "../PLAYER.HXX"		/* PlayerSpeed, PlayerHiresSpeed */
#include "../MENU.H"			/* sMenusUp */
#include "../SOUND.HXX"			/* TurnOffAllSounds, ShutDownSoundSystem */

/* REDBOOK.C; MACHINE.C declared these locally too. Every .C here builds
   as C++, so these stay C++-mangled - no extern "C". */
int InitRedBook(void);
int ShutdownRedBook(void);

/* ------------------------------------------------------------------------
   Globals that used to live in WINSYS
   ------------------------------------------------------------------------ */
LONG	WindowWidth  = MAX_VIEW_WIDTH;
LONG	WindowHeight = MAX_VIEW_HEIGHT;

SHORT	sDrawMode = iGDI;		/* there is no DirectDraw path here */
SHORT	sDrawType = iBLT;
BOOL	fDrawing  = FALSE;
BOOL	fQuitting = FALSE;
BOOL	fAppHasFocus = TRUE;
LONG	fps;

BOOL	JustLoadWad = FALSE;
BOOL	fSound = TRUE;
BOOL	fMusic = TRUE;

#if !defined(_RELEASE)
BOOL	gDontFight = FALSE;
#endif

/* sysgraph.cpp */
PTR			GDIscreen = NULL;
COLORSPEC	CurPal[256];
BOOL		fHighRes;
SHORT		iDispBuffer = fERROR;
LONG		disp_color;

/* mouse.c */
LONG	old_cursor_x, old_cursor_y;
LONG	cursor_x, cursor_y, mouse_button;
char	chCursorName[32];
LONG	location_x, location_y;
LONG	mouse_click = 0;
LONG	mouse_present = TRUE;
LONG	current_cursor;
LONG	need_cursor_draw = FALSE;
SHORT	cursor_bitmap;
BOOL	fAutoRestoreCursor = FALSE;

/* keyint.c */
volatile unsigned char	key_stat[256];
volatile BOOL			fAnyKeyChanged;

/* machine.c owned these too. */
int		iChatKey;
LONG	lSavedX = -1;
LONG	lSavedY = -1;

/* machine.c owned these; the joystick is not wired up on this backend. */
LONG	JoyXPos = 0, JoyYPos = 0, JoyZPos = 0;
BOOL	JoyButton1 = FALSE, JoyButton2 = FALSE;
BOOL	JoyButton3 = FALSE, JoyButton4 = FALSE;

extern HWND hwndApp;			/* GAME.CPP */
extern char pInitScene[];
extern BOOL fLogComment;

/* ------------------------------------------------------------------------
   SDL state
   ------------------------------------------------------------------------ */
static SDL_Window   *gWindow  = NULL;
static SDL_Renderer *gRender  = NULL;
static SDL_Texture  *gTexture = NULL;		/* 640x480  */
static SDL_Texture  *gTex2x   = NULL;		/* 1280x960, Scale2x output */

static UBYTE   *gFrameBuf = NULL;			/* the 8bpp game buffer      */
static UBYTE   *gScaleBuf = NULL;			/* 8bpp Scale2x scratch      */
static Uint32   gPalLut[256];				/* palette -> ARGB8888       */

static BOOL     fFadedOut = FALSE;
static BOOL     fWindowShown = FALSE;

/* Client-area size chosen on the options screen; the game buffer stays
   640x480 and the renderer scales it, exactly as the Win32 build does. */
static LONG GameWindowClientW = MAX_VIEW_WIDTH;
static LONG GameWindowClientH = MAX_VIEW_HEIGHT;

/* The DIB the Win32 build allocates is one row taller than the visible
   frame; some engine paths write a scratch row past the bottom. */
#define FRAME_ROWS	(MAX_VIEW_HEIGHT + 1)

/* ========================================================================
   Keyboard - the game indexes key_stat[] by Win32 virtual-key code, so
   the backend translates rather than teaching the core SDL scancodes.
   ======================================================================== */
static int VKFromSDL(SDL_Keycode k, SDL_Scancode sc)
{
	if (k >= SDLK_a && k <= SDLK_z)
		return 'A' + (k - SDLK_a);
	if (k >= SDLK_0 && k <= SDLK_9)
		return '0' + (k - SDLK_0);

	switch (k)
	{
	case SDLK_BACKSPACE:	return VK_BACK;
	case SDLK_TAB:			return VK_TAB;
	case SDLK_CLEAR:		return VK_CLEAR;
	case SDLK_RETURN:		return VK_RETURN;
	case SDLK_KP_ENTER:		return VK_RETURN;
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:		return VK_SHIFT;
	case SDLK_LCTRL:
	case SDLK_RCTRL:		return VK_CONTROL;
	case SDLK_LALT:
	case SDLK_RALT:			return VK_MENU;
	case SDLK_PAUSE:		return VK_PAUSE;
	case SDLK_CAPSLOCK:		return VK_CAPITAL;
	case SDLK_ESCAPE:		return VK_ESCAPE;
	case SDLK_SPACE:		return VK_SPACE;
	case SDLK_PAGEUP:		return VK_PRIOR;
	case SDLK_PAGEDOWN:		return VK_NEXT;
	case SDLK_END:			return VK_END;
	case SDLK_HOME:			return VK_HOME;
	case SDLK_LEFT:			return VK_LEFT;
	case SDLK_UP:			return VK_UP;
	case SDLK_RIGHT:		return VK_RIGHT;
	case SDLK_DOWN:			return VK_DOWN;
	case SDLK_PRINTSCREEN:	return VK_SNAPSHOT;
	case SDLK_INSERT:		return VK_INSERT;
	case SDLK_DELETE:		return VK_DELETE;
	case SDLK_HELP:			return VK_HELP;
	case SDLK_KP_MULTIPLY:	return VK_MULTIPLY;
	case SDLK_KP_PLUS:		return VK_ADD;
	case SDLK_KP_MINUS:		return VK_SUBTRACT;
	case SDLK_KP_PERIOD:	return VK_DECIMAL;
	case SDLK_KP_DIVIDE:	return VK_DIVIDE;
	case SDLK_NUMLOCKCLEAR:	return VK_NUMLOCK;
	case SDLK_SCROLLLOCK:	return VK_SCROLL;
	default:				break;
	}

	if (k >= SDLK_F1 && k <= SDLK_F12)
		return VK_F1 + (k - SDLK_F1);
	if (sc >= SDL_SCANCODE_KP_1 && sc <= SDL_SCANCODE_KP_0)
		return (sc == SDL_SCANCODE_KP_0) ? VK_NUMPAD0
										 : VK_NUMPAD1 + (sc - SDL_SCANCODE_KP_1);

	/* Anything else keeps its ASCII value, which is what the game's own
	   key tables assume for punctuation. */
	return (k > 0 && k < 128) ? (int)k : 0;
}

static void KeyDown(int vk)
{
	if (vk <= 0 || vk > 255)
		return;

	if (!(key_stat[vk] & 0x80))			/* if not prohibited */
	{
		fAnyKeyChanged = TRUE;
		key_stat[vk] = 0x81;			/* set key and prohibit */
	}
	key_stat[vk] |= 0x40;				/* key down */
}

static void KeyUp(int vk)
{
	if (vk <= 0 || vk > 255)
		return;

	key_stat[vk] &= 0x3F;				/* clear permit and keydown */
	fAnyKeyChanged = TRUE;
}

LONG key_status(LONG k)
{
	LONG rv;

	rv = key_stat[k] & 0x3F;
	key_stat[k] &= 0xC0;
	return rv;
}

LONG async_key_status(LONG k)
{
	return (key_stat[k] & 0x40);
}

void clear_key_status(LONG /*unused*/)
{
	ClearMessageQueue();
	memset((void *)key_stat, 0, sizeof(key_stat));
}

LONG keys_down(void)
{
	return FALSE;						/* matches WINSYS/KEYINT.C */
}

void install_keyint(void) {}
void remove_keyint(void) {}

short GetAsyncKeyState(int vk)
{
	if (vk == VK_LBUTTON)
		return (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) ? (short)0x8000 : 0;
	if (vk == VK_RBUTTON)
		return (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)) ? (short)0x8000 : 0;

	return (vk > 0 && vk < 256 && (key_stat[vk] & 0x40)) ? (short)0x8000 : 0;
}

/* ========================================================================
   Window-manager shims still called from the portable core
   ======================================================================== */
int GetClientRect(HWND /*hwnd*/, RECT *rc)
{
	int w = (int)GameWindowClientW, h = (int)GameWindowClientH;

	if (gWindow != NULL)
		SDL_GetWindowSize(gWindow, &w, &h);

	rc->left = rc->top = 0;
	rc->right  = w;
	rc->bottom = h;
	return TRUE;
}

int GetWindowRect(HWND hwnd, RECT *rc)
{
	int x = 0, y = 0;

	GetClientRect(hwnd, rc);
	if (gWindow != NULL)
		SDL_GetWindowPosition(gWindow, &x, &y);

	rc->left   += x;
	rc->top    += y;
	rc->right  += x;
	rc->bottom += y;
	return TRUE;
}

int ShowWindow(HWND /*hwnd*/, int cmd)
{
	if (gWindow == NULL)
		return FALSE;

	if (cmd == SW_HIDE)
	{
		SDL_HideWindow(gWindow);
		fWindowShown = FALSE;
	}
	else
	{
		SDL_ShowWindow(gWindow);
		fWindowShown = TRUE;
	}
	return TRUE;
}

HWND SetFocus(HWND hwnd)
{
	if (gWindow != NULL)
		SDL_RaiseWindow(gWindow);
	return hwnd;
}

int InvalidateRect(HWND /*hwnd*/, const RECT * /*rc*/, int /*erase*/)
{
	update_screen();
	return TRUE;
}

int WinHelp(HWND, const char *, unsigned, unsigned long)
{
	return FALSE;						/* no WinHelp on this platform */
}

/* ========================================================================
   Timing and errors (WINSYS/UTIL.C)
   ======================================================================== */
LONG get_time(void)
{
	return (LONG)(SDL_GetTicks() / 55);	/* a "tick" is 1/18s */
}

void pause(LONG t)
{
	LONG end = get_time() + t;

	while (get_time() < end)
		SDL_Delay(1);
}

ULONG get_free_mem(void)
{
	return (ULONG)(8 * 1024 * 1024);
}

char *safe_malloc(LONG size)
{
	char *x = (char *)malloc(size);

	if (x == NULL)
		fatal_error("Cant Malloc memory in safe_malloc");
	return x;
}

void fatal_error(const char *format, ...)
{
	char	texbuffer[200];
	FILE   *f;
	va_list	argp;

	va_start(argp, format);
	vsnprintf(texbuffer, sizeof(texbuffer), format, argp);
	va_end(argp);

	close_graph();
	remove_keyint();

	printf("Fatal Error:%s\n", texbuffer);
	fflush(stdout);

	f = fopen("Fatal.err", "wt");
	if (f != NULL)
	{
		fprintf(f, "Fatal Error:%s\n", texbuffer);
		fclose(f);
	}

	if (gWindow != NULL)
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "FATAL ERROR", texbuffer, gWindow);

	exit(0);
}

void UpdateFPS(void)
{
	static LONG lastTime = 0;
	static LONG frames = 0;
	LONG curTime = (LONG)SDL_GetTicks();

	frames++;
	if (curTime - 1000 > lastTime)
	{
		fps = frames;
		lastTime = curTime;
		frames = 0;
	}
}

/* ========================================================================
   Graphics (WINSYS/SYSGRAPH.CPP)
   ======================================================================== */
void set_hires(LONG, LONG)
{
	LONG NewMapCX, NewMapCY;

	if (fHighRes == FALSE)
	{
		/* The buffer is always 640x480; the window may be larger and the
		   renderer scales.  Never derive the buffer size from the window. */
		WindowWidth  = MAX_VIEW_WIDTH;
		WindowHeight = MAX_VIEW_HEIGHT;

		fHighRes = TRUE;
		set_screen_size(WindowWidth, WindowHeight);

		set_mouse_max();

		ZoomMapAbsolute(0, MapZoomFactor() / 2);

		NewMapCX = MapCenterX() / 2;
		NewMapCY = MapCenterY() * 2;
		SetMapCenter(NewMapCX, NewMapCY);

		PlayerSpeed = PlayerHiresSpeed;
	}
}

void set_lowres(LONG, LONG)
{
	LONG NewMapCX, NewMapCY;

	if (sMenusUp != 0)
		return;

	if (fHighRes == TRUE)
	{
		WindowWidth  /= 2;
		WindowHeight /= 2;

		fHighRes = FALSE;
		set_screen_size(WindowWidth, WindowHeight);

		set_mouse_max();

		ZoomMapAbsolute(0, MapZoomFactor() * 2);

		NewMapCX = MapCenterX() * 2;
		NewMapCY = MapCenterY() / 2;
		SetMapCenter(NewMapCX, NewMapCY);

		PlayerSpeed = (SHORT)PlayerLoresSpeed;
	}
}

/* Nothing pages video memory any more; kept because MACHINE.H declares
   them and a couple of DOS-era call sites still reference them. */
void set_svga_page(ULONG) {}
LONG CalcPage(void) { return 0; }

static BOOL CreatePresentTargets(void)
{
	SDL_RendererInfo info;

	gTexture = SDL_CreateTexture(gRender, SDL_PIXELFORMAT_ARGB8888,
								 SDL_TEXTUREACCESS_STREAMING,
								 MAX_VIEW_WIDTH, MAX_VIEW_HEIGHT);
	if (gTexture == NULL)
		return FALSE;

	/* Scale2x target. Optional: without it the renderer just scales the
	   640x480 frame directly, which is what happens at 1:1 anyway. */
	gTex2x = SDL_CreateTexture(gRender, SDL_PIXELFORMAT_ARGB8888,
							   SDL_TEXTUREACCESS_STREAMING,
							   MAX_VIEW_WIDTH * 2, MAX_VIEW_HEIGHT * 2);
	if (gTex2x != NULL)
	{
		gScaleBuf = (UBYTE *)calloc(1, MAX_VIEW_WIDTH * 2 * MAX_VIEW_HEIGHT * 2);
		if (gScaleBuf == NULL)
		{
			SDL_DestroyTexture(gTex2x);
			gTex2x = NULL;
		}
	}

	if (SDL_GetRendererInfo(gRender, &info) == 0)
		printf("SDL renderer: %s\n", info.name);

	return TRUE;
}

void init_graph(LONG init_mode)
{
	if (init_mode)
	{
		fHighRes = FALSE;				/* guarantee the resolution changes */
		set_hires(0L, 0L);
	}
	else
	{
		fHighRes = TRUE;
		set_lowres(0L, 0L);
	}

	init_pal("default");

	if (gFrameBuf == NULL)
	{
		gFrameBuf = (UBYTE *)calloc(1, MAX_VIEW_WIDTH * FRAME_ROWS);
		if (gFrameBuf == NULL)
			fatal_error("init_graph: out of memory for the frame buffer");
	}

	screen    = (PTR)gFrameBuf;
	GDIscreen = (PTR)gFrameBuf;

	iDispBuffer = NewExternalBlock(screen);

	if (gRender != NULL && gTexture == NULL)
		CreatePresentTargets();

	clear_screen();

	init_shade_table("default");
}

void close_graph(void)
{
	if (gTexture != NULL)
	{
		SDL_DestroyTexture(gTexture);
		gTexture = NULL;
	}
	if (gTex2x != NULL)
	{
		SDL_DestroyTexture(gTex2x);
		gTex2x = NULL;
	}
	free(gScaleBuf);
	gScaleBuf = NULL;
}

void set_pal(char *pal)
{
	int i;

	for (i = 0; i < 256; i++)
	{
		run_timers();
		gPalLut[i] = 0xFF000000u
				   | ((Uint32)(UBYTE)pal[(i * 3)]     << 16)
				   | ((Uint32)(UBYTE)pal[(i * 3) + 1] <<  8)
				   |  (Uint32)(UBYTE)pal[(i * 3) + 2];
	}

	/* The Win32 build forces these two entries; palette-animated art
	   depends on index 0 being black and 255 being white. */
	gPalLut[0]   = 0xFF000000u;
	gPalLut[255] = 0xFFFFFFFFu;
}

void load_pal(char *n)
{
	FILE *f = FileOpen(n, "rb");

	if (f == NULL)
		fatal_error("Palette not found: %s", n);

	/* .col files carry an 8-byte header before the 256 RGB triples */
	fread(&CurPal[0], sizeof(char), 8, f);
	fread(&CurPal[0], sizeof(char), 768, f);
	FileClose(f);

	set_pal((char *)&CurPal[0]);
}

void init_pal(char *PalName)
{
	char pathbuffer[80];

	sprintf(pathbuffer, "graphics\\%s.col", PalName);
	load_pal(pathbuffer);
}

/* AdvMAME2x, byte-for-byte the same algorithm as the GDI path so both
   backends produce identical pixels. */
static void Scale2xFrame(LONG srcw, LONG srch)
{
	LONG x, y;

	for (y = 0; y < srch; y++)
	{
		const UCHAR *row  = (const UCHAR *)gFrameBuf + y * MAX_VIEW_WIDTH;
		const UCHAR *up   = (y > 0)        ? row - MAX_VIEW_WIDTH : row;
		const UCHAR *down = (y < srch - 1) ? row + MAX_VIEW_WIDTH : row;
		UCHAR *out0 = gScaleBuf + (2 * y) * (MAX_VIEW_WIDTH * 2);
		UCHAR *out1 = out0 + (MAX_VIEW_WIDTH * 2);

		for (x = 0; x < srcw; x++)
		{
			UCHAR P = row[x];
			UCHAR A = up[x];
			UCHAR D = down[x];
			UCHAR C = (x > 0)        ? row[x - 1] : P;
			UCHAR B = (x < srcw - 1) ? row[x + 1] : P;
			UCHAR e0 = P, e1 = P, e2 = P, e3 = P;

			if (A != D && C != B)
			{
				if (C == A) e0 = A;
				if (A == B) e1 = B;
				if (C == D) e2 = C;
				if (B == D) e3 = D;
			}

			out0[2 * x]     = e0;
			out0[2 * x + 1] = e1;
			out1[2 * x]     = e2;
			out1[2 * x + 1] = e3;
		}
	}
}

/* Expand an 8bpp buffer through the palette LUT straight into a locked
   streaming texture. */
static void BlitIndexed(SDL_Texture *tex, const UBYTE *src, int w, int h)
{
	void *pixels = NULL;
	int   pitch  = 0;
	int   x, y;

	if (SDL_LockTexture(tex, NULL, &pixels, &pitch) != 0)
		return;

	for (y = 0; y < h; y++)
	{
		const UBYTE *s = src + (size_t)y * w;
		Uint32      *d = (Uint32 *)((Uint8 *)pixels + (size_t)y * pitch);

		for (x = 0; x < w; x++)
			d[x] = gPalLut[s[x]];
	}

	SDL_UnlockTexture(tex);
}

void update_screen(void)
{
	SDL_Texture *tex;
	int cw = 0, ch = 0;

	if (gRender == NULL || gFrameBuf == NULL)
		return;

	SDL_GetWindowSize(gWindow, &cw, &ch);

	if (gScaleBuf != NULL && (cw > WindowWidth || ch > WindowHeight))
	{
		Scale2xFrame(WindowWidth, WindowHeight);
		BlitIndexed(gTex2x, gScaleBuf, MAX_VIEW_WIDTH * 2, MAX_VIEW_HEIGHT * 2);
		tex = gTex2x;
	}
	else
	{
		BlitIndexed(gTexture, gFrameBuf, MAX_VIEW_WIDTH, MAX_VIEW_HEIGHT);
		tex = gTexture;
	}

	SDL_RenderClear(gRender);
	SDL_RenderCopy(gRender, tex, NULL, NULL);
	SDL_RenderPresent(gRender);
}

/* The GDI backend clips a dirty rect and re-presents the whole frame so
   partial updates stay bit-identical to update_screen.  A renderer has
   no cheaper partial path, so this is just the full present.
   ponytail: no dirty-rect tracking; add one only if presenting shows up
   in a profile. */
void ScreenCopy(SHORT, SHORT, SHORT, SHORT, SHORT, SHORT)
{
	update_screen();
}

void clear_screen(void)
{
	if (screen != NULL)
		memset(screen, 0, MAX_VIEW_WIDTH * MAX_VIEW_HEIGHT);
}

void clear_screen_to(int c)
{
	if (screen != NULL)
		memset(screen, c, MAX_VIEW_WIDTH * MAX_VIEW_HEIGHT);
}

void clear_display(void)
{
	clear_screen();
}

void FadeOut(USHORT steps)
{
	COLORSPEC	TempPal[256];
	SHORT		i, j;

	if (fFadedOut == TRUE)
		return;

	fFadedOut = TRUE;
	fIsFadedOut = TRUE;

	steps /= 32;

	if (steps)
	{
		for (i = steps; i >= 0; i--)
		{
			for (j = 0; j < 256; j++)
			{
				TempPal[j].bRed   = (CurPal[j].bRed   * i) / steps;
				TempPal[j].bGreen = (CurPal[j].bGreen * i) / steps;
				TempPal[j].bBlue  = (CurPal[j].bBlue  * i) / steps;
				run_timers();
			}
			set_pal((char *)TempPal);
			/* GDI realized the palette on the live screen, so the fade was
			   visible without redrawing.  Here the LUT only reaches the
			   window through a present. */
			update_screen();
		}
	}
	else
	{
		memset((PTR)TempPal, 0, 768);
		set_pal((char *)TempPal);
		update_screen();
	}
}

void FadeIn(USHORT steps)
{
	COLORSPEC	TempPal[256];
	USHORT		i, j;

	if (fFadedOut == FALSE)
		return;

	fFadedOut = FALSE;
	fIsFadedOut = FALSE;

	steps /= 32;

	if (steps)
	{
		for (i = 0; i <= steps; i++)
		{
			for (j = 0; j < 256; j++)
			{
				TempPal[j].bRed   = (CurPal[j].bRed   * i) / steps;
				TempPal[j].bGreen = (CurPal[j].bGreen * i) / steps;
				TempPal[j].bBlue  = (CurPal[j].bBlue  * i) / steps;
				run_timers();
			}
			set_pal((char *)TempPal);
			update_screen();
		}
	}
	else
	{
		set_pal((char *)CurPal);
		update_screen();
	}
}

/* ========================================================================
   Mouse (WINSYS/MOUSE.C)
   ======================================================================== */
void init_mouse(CSTRPTR name, BOOL /*Centered*/)
{
	char path[256];

	strcpy(chCursorName, name);

	sprintf(path, "graphics\\%s.pcx", name);
	cursor_bitmap = GetResourceStd(path, FALSE);
	if (cursor_bitmap == fERROR)
		fatal_error("init_mouse: file %s not found", name);

	set_mouse_max();
}

void set_mouse_max(void) {}
void set_mouse_position(LONG, LONG) {}
void WinCursorSet(void) {}

void read_mouse(void)
{
	RECT rc;
	int  x = 0, y = 0;
	Uint32 buttons;

	GetClientRect(hwndApp, &rc);

	if (rc.right && rc.bottom)
	{
		buttons = SDL_GetMouseState(&x, &y);

		/* scale window coordinates back onto the 640x480 game buffer */
		cursor_x = (LONG)x * WindowWidth  / rc.right;
		cursor_y = (LONG)y * WindowHeight / rc.bottom;

		mouse_button = FALSE;

		if (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT))
			mouse_button = 2;
		else if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
			mouse_button = 1;
	}
}

void update_buttons(void)
{
	Uint32 buttons = SDL_GetMouseState(NULL, NULL);

	if (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT))
		mouse_button = 2;
	else if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
		mouse_button = 1;
}

/* The 1997 software cursor (blit the cursor bitmap into the frame, then
   restore what was under it) only ran in the DirectDraw path. GDI mode
   let the OS draw the pointer, and so does SDL - so these are the same
   no-ops WINSYS/MOUSE.C reduced to when sDrawMode == iGDI. */
void SysHideCursor(void)  {}
void SysShowCursor(void)  {}
void SysForceCursor(void) {}

void draw_cursor(void)
{
	read_mouse();
}

LONG cursor_in_box(LONG x1, LONG y1, LONG w, LONG h)
{
	return (cursor_x > x1 && cursor_y > y1 && cursor_x < x1 + w && cursor_y < y1 + h);
}

/* ========================================================================
   Window sizing (WINSYS/MACHINE.C)
   ======================================================================== */
void DefaultMainWindowSize(UINT /*flags*/)
{
	SDL_DisplayMode dm;

	if (gWindow == NULL)
		return;

	SDL_SetWindowSize(gWindow, (int)GameWindowClientW, (int)GameWindowClientH);

	/* Cover the whole desktop -> go borderless, as the Win32 build does. */
	if (SDL_GetDesktopDisplayMode(0, &dm) == 0 &&
		GameWindowClientW >= dm.w && GameWindowClientH >= dm.h)
	{
		SDL_SetWindowBordered(gWindow, SDL_FALSE);
		SDL_SetWindowPosition(gWindow, 0, 0);
	}
	else
	{
		SDL_SetWindowBordered(gWindow, SDL_TRUE);
		SDL_SetWindowPosition(gWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}
}

void SetGameResolution(LONG w, LONG h)
{
	if (w == GameWindowClientW && h == GameWindowClientH)
		return;

	GameWindowClientW = w;
	GameWindowClientH = h;

	DefaultMainWindowSize(0);
	update_screen();
}

/* ========================================================================
   Event pump - the SDL equivalent of the Win32 message loop
   ======================================================================== */
static void HandleEvent(const SDL_Event *ev)
{
	switch (ev->type)
	{
	case SDL_QUIT:
		quit_sys(0);
		break;

	case SDL_KEYDOWN:
		KeyDown(VKFromSDL(ev->key.keysym.sym, ev->key.keysym.scancode));
		break;

	case SDL_KEYUP:
		KeyUp(VKFromSDL(ev->key.keysym.sym, ev->key.keysym.scancode));
		break;

	case SDL_MOUSEBUTTONDOWN:
		if (ev->button.button == SDL_BUTTON_LEFT)
			mouse_button = 1;
		else if (ev->button.button == SDL_BUTTON_RIGHT)
			mouse_button = 2;
		break;

	case SDL_WINDOWEVENT:
		switch (ev->window.event)
		{
		case SDL_WINDOWEVENT_FOCUS_LOST:
			fAppHasFocus = FALSE;
			break;
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			fAppHasFocus = TRUE;
			update_screen();
			break;
		case SDL_WINDOWEVENT_EXPOSED:
			update_screen();
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}
}

void WindowsMessages(void)
{
	SDL_Event ev;

	while (!fQuitting && SDL_PollEvent(&ev))
		HandleEvent(&ev);
}

void ClearMessageQueue(void)
{
	SDL_Event ev;

	while (SDL_PollEvent(&ev))
		HandleEvent(&ev);
}

void AppIdle(void)
{
	if (!fQuitting && fAppHasFocus)
		MainLoop();

	WindowsMessages();
}

/* ========================================================================
   Startup / shutdown
   ======================================================================== */
void quit_sys(LONG)
{
	StopRedBook();
	TurnOffAllSounds();
	ShutDownSoundSystem();
	CloseAllFiles();
	fQuitting = TRUE;
}

static void AppExit(void)
{
	close_graph();
	remove_keyint();
	TurnOffAllSounds();
	ShutdownRedBook();
	QuitMemManag();

	free(gFrameBuf);
	gFrameBuf = NULL;

	if (gRender != NULL)
		SDL_DestroyRenderer(gRender);
	if (gWindow != NULL)
		SDL_DestroyWindow(gWindow);

	SDL_Quit();
}

/* brsetup.cfg drives the install type and CD path, exactly as on Win32. */
static BOOL ReadSetupCfg(void)
{
	FILE *fp = fopen("brsetup.cfg", "r");
	char  cpBuffer[256];
	LONG  fileResult;

	if (fp == NULL)
		return FALSE;

	InstallationType = 2;					/* English default */

	do
	{
		fileResult = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
		if (fileResult != EOF)
		{
			if (strcmp(cpBuffer, "[LOGCOMMENT]") == 0)
			{
				fLogComment = TRUE;
			}
			else if (strcmp(cpBuffer, "[INSTALL_TYPE]") == 0)
			{
				fileResult = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
				InstallationType = atoi(cpBuffer);
			}
			else if (strcmp(cpBuffer, "[CD]") == 0)
			{
				fileResult = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
				strcpy(CDDrive, cpBuffer);
				strcat(CDDrive, ":\\");
			}
		}
	} while (fileResult != EOF);

	strcpy(InstallPath, ".\\");
	fclose(fp);
	return TRUE;
}

static BOOL AppInit(void)
{
	install_keyint();
	CheckHandles();							/* FOPEN.C */

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
	{
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		return FALSE;
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");	/* linear, ~= HALFTONE */

	/* Created hidden: the game reveals the window itself from
	   UpdateOptionSettings once the saved resolution is applied. */
	gWindow = SDL_CreateWindow("Birthright - The Gorgon's Alliance",
							   SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							   (int)GameWindowClientW, (int)GameWindowClientH,
							   SDL_WINDOW_HIDDEN);
	if (gWindow == NULL)
	{
		fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
		return FALSE;
	}

	gRender = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRender == NULL)
		gRender = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_SOFTWARE);
	if (gRender == NULL)
	{
		fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
		return FALSE;
	}

	CreatePresentTargets();

	hwndApp = (HWND)gWindow;

	srand((unsigned)SDL_GetTicks());

	if (!ReadSetupCfg())
	{
		fprintf(stderr, "Unable to open brsetup.cfg. Game not properly installed.\n");
		return FALSE;
	}

	GameMain();

	return TRUE;
}

int main(int argc, char *argv[])
{
	parse_cmdline(argc, argv);

	if (!AppInit())
	{
		fprintf(stderr, "Can't initialize BirthRight.\n");
		return 1;
	}

	while (!fQuitting)
		AppIdle();

	AppExit();
	return 0;
}

void parse_cmdline(int argc, char *argv[])
{
	int i;

	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-wad") == 0)
			JustLoadWad = TRUE;
	}
}
