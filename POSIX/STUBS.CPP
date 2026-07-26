/* ========================================================================
   STUBS.CPP - the Windows-only subsystems the portable core still calls.

   DirectDraw, MCI (CD audio and AVI playback), the wave-device probe and
   the URL launcher.  None of them have a Linux equivalent that matters
   for getting the game up: the SDL backend owns presentation, and the
   1997 CD-audio path already degrades to silence when no CD is present
   (see REDBOOK.C), which is exactly the state these stubs report.
   ======================================================================== */
#include "../PLATFORM.H"

#include <stdio.h>

#include "../TYPEDEFS.H"
#include "../SYSTEM.H"
#include "../MACHINE.H"
#include "../MACHINT.H"

/* DDRAWPRO.H wraps its prototypes in extern "C", so these must match.
   MCIUTIL.H and GOTOURL.H do not - and every .C in this project builds as
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

/* -- MCI ----------------------------------------------------------------
   mci_OpenCD failing is the "no CD drive" path the game already handles:
   fCDAvailible stays FALSE and music falls back to MUSIC\<n>.WAV. */
DWORD mci_OpenCD(void)				{ return 1; }		/* non-zero == error */
DWORD mci_CloseCD(void)				{ return 0; }
DWORD mci_StopCD(void)				{ return 0; }
DWORD mci_CheckCDBusy(void)			{ return 0; }
DWORD mci_PlayCD(HWND, BYTE, BYTE, BYTE, BYTE, BYTE, BYTE, BYTE)
									{ return 1; }

BOOL mci_OpenAVI(void)				{ return FALSE; }
void mci_CloseAVI(void)				{}
BOOL mci_PlayAVI(char *, HWND)		{ return FALSE; }
void mci_StopAVI(void)				{}

DWORD mci_Init(void)				{ return 1; }
DWORD mci_OpenFile(LPSTR)			{ return 1; }
DWORD mci_OpenDevice(LPSTR)			{ return 1; }
DWORD mci_Close(void)				{ return 0; }
DWORD mci_Stop(void)				{ return 0; }
DWORD mci_Rewind(void)				{ return 0; }
DWORD mci_Play(void)				{ return 1; }
DWORD mci_GetPosition(void)			{ return 0; }

BOOL GotoURL(const char *, int)		{ return FALSE; }
BOOL CheckURL(const char *)			{ return FALSE; }
