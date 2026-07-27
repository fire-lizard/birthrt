/* ========================================================================
   PLATFORM.H - the single seam between the game and the host OS.

   Every translation unit that used to reach for <Windows.h> includes this
   instead. On Windows it is a thin pass-through, so the Win32 build is
   unchanged. On Linux/macOS it supplies the handful of Win32 types and
   calls the portable core actually uses.

   Genuinely Windows-only code (DirectDraw/DirectSound/DirectPlay, the
   Win32 message loop, the character-editor dialogs) is excluded from the
   POSIX build in CMakeLists.txt rather than being #ifdef'd inline.
   ======================================================================== */
#ifndef _PLATFORM_H
#define _PLATFORM_H

#if defined(_WIN32)

#include <Windows.h>
#include <io.h>
#include <direct.h>

#define PLAT_PATH_SEP		'\\'
#define plat_stricmp		_stricmp
#define plat_strupr			_strupr
#define plat_mkdir(p)		_mkdir(p)
#define plat_filelength(fd)	_filelength(fd)
#define plat_access			_access
#define plat_remove			remove

/* The game's own path strings are already Win32-shaped. */
#define PlatResolvePath(dst, src, len)	((void)plat_copy_path((dst), (src), (len)))

#else	/* ---------------- Linux / macOS ---------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>

#if !defined(_TYPEDEFS_H)
#include "TYPEDEFS.H"
#endif

/* -- opaque window/instance handles; the POSIX build has no HWNDs -------- */
typedef void *	HWND;
typedef void *	HINSTANCE;
typedef void *	HDC;
typedef void *	HANDLE;
typedef void *	HCURSOR;
typedef void *	HCURSOR__;
typedef void *	HMENU;
typedef void *	HBITMAP;
typedef void *	HPALETTE;
typedef void *	HKEY;
typedef void *	LPVOID;
typedef char *	LPSTR;
typedef char *	PSZ;
typedef char *	PSTR;
typedef char *	PCHAR;
typedef DWORD *	LPDWORD;
typedef const char *	LPCSTR;
typedef unsigned short	WORD;
typedef unsigned long	ULONG_PTR;
typedef ULONG_PTR	WPARAM;
typedef long		LPARAM;
typedef long		HRESULT;

#define WINAPI
#define CALLBACK
#define FAR
#define PASCAL
#define far
#define near
#define __declspec(x)
#define INVALID_HANDLE_VALUE	((HANDLE)-1)

/* Virtual-key codes. The game stores key state in key_stat[] indexed by
   VK code (KEYINT.C), so the SDL backend translates scancodes back into
   these rather than the codebase learning a second key namespace. */
#define VK_LBUTTON	0x01
#define VK_RBUTTON	0x02
#define VK_CANCEL	0x03
#define VK_MBUTTON	0x04
#define VK_BACK		0x08
#define VK_TAB		0x09
#define VK_CLEAR	0x0C
#define VK_RETURN	0x0D
#define VK_SHIFT	0x10
#define VK_CONTROL	0x11
#define VK_MENU		0x12
#define VK_PAUSE	0x13
#define VK_CAPITAL	0x14
#define VK_ESCAPE	0x1B
#define VK_SPACE	0x20
#define VK_PRIOR	0x21
#define VK_NEXT		0x22
#define VK_END		0x23
#define VK_HOME		0x24
#define VK_LEFT		0x25
#define VK_UP		0x26
#define VK_RIGHT	0x27
#define VK_DOWN		0x28
#define VK_SELECT	0x29
#define VK_PRINT	0x2A
#define VK_EXECUTE	0x2B
#define VK_SNAPSHOT	0x2C
#define VK_INSERT	0x2D
#define VK_DELETE	0x2E
#define VK_HELP		0x2F
#define VK_NUMPAD0	0x60
#define VK_NUMPAD1	0x61
#define VK_NUMPAD2	0x62
#define VK_NUMPAD3	0x63
#define VK_NUMPAD4	0x64
#define VK_NUMPAD5	0x65
#define VK_NUMPAD6	0x66
#define VK_NUMPAD7	0x67
#define VK_NUMPAD8	0x68
#define VK_NUMPAD9	0x69
#define VK_MULTIPLY	0x6A
#define VK_ADD		0x6B
#define VK_SEPARATOR	0x6C
#define VK_SUBTRACT	0x6D
#define VK_DECIMAL	0x6E
#define VK_DIVIDE	0x6F
#define VK_F1		0x70
#define VK_F2		0x71
#define VK_F3		0x72
#define VK_F4		0x73
#define VK_F5		0x74
#define VK_F6		0x75
#define VK_F7		0x76
#define VK_F8		0x77
#define VK_F9		0x78
#define VK_F10		0x79
#define VK_F11		0x7A
#define VK_F12		0x7B
#define VK_NUMLOCK	0x90
#define VK_SCROLL	0x91
#define VK_ZOOM		0xFB

#ifndef MAX_PATH
#define MAX_PATH	PATH_MAX
#endif
#ifndef _MAX_PATH
#define _MAX_PATH	PATH_MAX
#endif

/* <unistd.h> above declares `int pause(void)`, which collides with the
   game's own `void pause(LONG)` (MACHINE.H) - both are extern "C", so the
   linkage clashes rather than overloading. Rename the game's, after the
   system header has been seen. */
#define pause		birthrt_pause

/* MSVC CRT spellings used throughout the 1997 sources. Defined after the
   system headers above so the real declarations are not rewritten. */
#define stricmp		strcasecmp
#define strnicmp	strncasecmp
#define strupr		plat_strupr
#define filelength	plat_filelength
#if defined(__cplusplus)
/* libstdc++'s <cstdio> and friends #undef the C library names when they are
   first included, which would silently drop the redirects below in any TU
   that pulls in a C++ header after this one - and every .C here compiles as
   C++. Getting those includes over with now makes the redirects stick. */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#endif

/* The game reaches for these directly with Win32-shaped paths - saves go to
   "savegame\\current.sav", mkdir("SaveGame") - and only FOPEN.C's FileOpen
   ever ran them through the resolver. On Linux that wrote a file literally
   named "savegame\current.sav" next to a new "SaveGame" directory, instead
   of current.sav inside the shipped SAVEGAME. Route them all through the
   same seam; PLATFORM.CPP #undefs these before implementing them.

   `remove` deliberately stays a plain function: <algorithm> declares
   std::remove, so a macro of that name breaks every C++ header that sees it.
   The few call sites that delete a game path call plat_remove directly. */
#define fopen(p, m)	plat_fopen((p), (m))
#define access(p, m)	plat_access_path((p), (m))
#define mkdir(p)	plat_mkdir_path(p)

/* Win32 odds and ends the core still references. */
#define HELP_CONTENTS	0x0003
#define HELP_CONTEXT	0x0001

#ifndef O_BINARY
#define O_BINARY	0
#endif

/* MessageBox styles the game passes; the POSIX build only logs. */
#define MB_OK				0x0000
#define MB_OKCANCEL			0x0001
#define MB_YESNO			0x0004
#define MB_ICONHAND			0x0010
#define MB_ICONEXCLAMATION	0x0030
#define MB_ICONINFORMATION	0x0040
#define MB_ICONSTOP			MB_ICONHAND
#define MB_TASKMODAL		0x2000
#define IDOK	1
#define IDYES	6
#define IDNO	7

#define PLAT_PATH_SEP		'/'
#define plat_stricmp		strcasecmp
#define plat_mkdir(p)		plat_mkdir_path(p)
#define plat_access			plat_access_path

/* Game paths are Win32-shaped ("GRAPHICS\\DEFAULT.COL") and their case
   rarely matches what is on disk. Rewrite them before they reach the
   filesystem; safe to call with dst == src. */
#define PlatResolvePath(dst, src, len)	((void)plat_copy_path((dst), (src), (len)))

#if defined(__cplusplus)
extern "C" {
#endif

/* -- the Win32 calls the portable core still makes ----------------------- */
#define SW_HIDE		0
#define SW_SHOW		5
#define HELP_QUIT	0x0002

void	Sleep(unsigned long ms);
unsigned long GetTickCount(void);
int		MessageBox(HWND hwnd, const char *text, const char *caption, unsigned type);
void	OutputDebugString(const char *s);
int		wsprintf(char *dst, const char *fmt, ...);
char *	plat_strupr(char *s);
long	plat_filelength(int fd);

/* Implemented by the SDL backend (POSIX/SDLSYS.CPP); these are the only
   window-manager calls left in the portable core. */
int		ShowWindow(HWND hwnd, int cmd);
HWND	SetFocus(HWND hwnd);
int		InvalidateRect(HWND hwnd, const RECT *rc, int erase);
int		GetWindowRect(HWND hwnd, RECT *rc);
int		GetClientRect(HWND hwnd, RECT *rc);
short	GetAsyncKeyState(int vk);
int		WinHelp(HWND hwnd, const char *help, unsigned cmd, unsigned long data);
unsigned int waveOutGetNumDevs(void);
int		ShowCursor(int show);
int		AllocConsole(void);

#if defined(__cplusplus)
}

/* Not extern "C": POSIX/STUBS.CPP defines it outside the block, matching
   GOTOURL.H and the other WINSYS headers that never wrapped theirs. */
HINSTANCE FindExecutable(const char *file, const char *dir, char *result);
#endif

#endif	/* _WIN32 */

/* ------------------------------------------------------------------------
   Portable helpers - available on every target.
   ------------------------------------------------------------------------ */

/* Bitfield layout is NOT the same on MSVC and gcc. MSVC starts a new
   storage unit when the next bitfield will not fit in the current one;
   gcc (under #pragma pack) packs bits contiguously across byte
   boundaries. Structs that reach a save file must use the MSVC rule or
   the 1997 file format silently changes size - sizeof(REGENT) came out
   12 instead of 14. Tag those structs with this. */
#if defined(_WIN32)
#define MS_BITFIELDS
#else
#define MS_BITFIELDS	__attribute__((ms_struct))
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/* Rewrites a game path ("UI\\HSTBTN1A.PCX") into something the host
   filesystem will actually open: separators normalised, and on a
   case-sensitive filesystem each component matched case-insensitively
   against what is on disk. Returns dst. */
char *plat_copy_path(char *dst, const char *src, unsigned long len);

#if !defined(_WIN32)
/* The four bare filesystem calls the game makes with unresolved game paths.
   Each resolves its argument, then does what its namesake does. */
FILE *plat_fopen(const char *path, const char *mode);
int   plat_access_path(const char *path, int mode);
int   plat_remove(const char *path);
int   plat_mkdir_path(const char *path);
#endif

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)
#include <string>
#include <vector>
/* Replaces the FindFirstFile/FindNextFile walk in FINDFILE.CPP and
   LOADSAVE.CPP. `pattern` is a game path that may end in a * wildcard. */
std::vector<std::string> PlatListFiles(const char *pattern);
#endif

#endif	/* _PLATFORM_H */
