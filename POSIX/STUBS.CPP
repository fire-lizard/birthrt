/* ========================================================================
   STUBS.CPP - the Windows-only subsystems the portable core still calls.

   DirectDraw, the wave-device probe and the URL launcher.  None of them
   have a Linux equivalent that matters for getting the game up: the SDL
   backend owns presentation.
   ======================================================================== */
#include "../PLATFORM.H"

#include <stdio.h>

#include "../TYPEDEFS.H"
#include "../SYSTEM.H"
#include "../MACHINE.H"
#include "../MACHINT.H"

/* DDRAWPRO.H wraps its prototypes in extern "C", so these must match.
   GOTOURL.H does not - and every .C in this project builds as
   C++ - so those stay C++-mangled below. Get this wrong and it shows up
   as an undefined symbol at link time, not a compile error. */
extern "C" {

/* -- DirectDraw ---------------------------------------------------------
   sDrawMode is pinned to iGDI on this backend, so every one of these is
   dead code the linker still needs a body for. */
BOOL DDInitialize(HWND, BOOL)		{ return FALSE; }
void DDCleanup(void)				{}
HRESULT DDSetCooperative(BOOL)		{ return 0; }

/* -- misc ---------------------------------------------------------------
   SOUND.CPP uses this only to decide whether a sound device exists; the
   SDL audio backend will answer for itself, so claim one device. */
unsigned int waveOutGetNumDevs(void)	{ return 1; }

/* SDL owns the cursor here, and this process already has a terminal. */
int ShowCursor(int)						{ return 0; }
int AllocConsole(void)					{ return 1; }

}	/* extern "C" */

/* MULTIUI.CPP compares the result against 32 the old ShellExecute way;
   anything <= 32 means "could not launch", which is the honest answer. */
HINSTANCE FindExecutable(const char *, const char *, char *)
{
	return (HINSTANCE)0;
}

void DirectDrawSaveScreen(void)		{}

BOOL GotoURL(const char *, int)		{ return FALSE; }
BOOL CheckURL(const char *)			{ return FALSE; }
