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
#include "typedefs.h"
#endif

/* -- opaque window/instance handles; the POSIX build has no HWNDs -------- */
typedef void *	HWND;
typedef void *	HINSTANCE;
typedef void *	HDC;
typedef void *	HANDLE;
typedef void *	HCURSOR;
typedef void *	HMENU;
typedef void *	HBITMAP;
typedef void *	LPVOID;
typedef char *	LPSTR;
typedef const char *	LPCSTR;
typedef unsigned short	WORD;
typedef unsigned long	ULONG_PTR;
typedef ULONG_PTR	WPARAM;
typedef long		LPARAM;

#define WINAPI
#define CALLBACK
#define FAR
#define PASCAL
#define __declspec(x)
#define INVALID_HANDLE_VALUE	((HANDLE)-1)

#ifndef MAX_PATH
#define MAX_PATH	PATH_MAX
#endif

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
#define plat_mkdir(p)		mkdir((p), 0777)
#define plat_access			access

#if defined(__cplusplus)
extern "C" {
#endif

/* -- the Win32 calls the portable core still makes ----------------------- */
void	Sleep(unsigned long ms);
unsigned long GetTickCount(void);
int		MessageBox(HWND hwnd, const char *text, const char *caption, unsigned type);
void	OutputDebugString(const char *s);
int		wsprintf(char *dst, const char *fmt, ...);
char *	plat_strupr(char *s);
long	plat_filelength(int fd);

#if defined(__cplusplus)
}
#endif

#endif	/* _WIN32 */

/* ------------------------------------------------------------------------
   Portable helpers - available on every target.
   ------------------------------------------------------------------------ */
#if defined(__cplusplus)
extern "C" {
#endif

/* Rewrites a game path ("UI\\HSTBTN1A.PCX") into something the host
   filesystem will actually open: separators normalised, and on a
   case-sensitive filesystem each component matched case-insensitively
   against what is on disk. Returns dst. */
char *plat_copy_path(char *dst, const char *src, unsigned long len);

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
