/* ========================================================================
   SDLAUDIO.HPP - the POSIX counterpart of SOLAUDIO/DSOUND.HPP.

   KERNAUD.CPP holds all the portable audio logic (wave lookup, the 8 slot
   table, RIFF parsing) and drives one DSound object per sample.  This
   header declares exactly the DSound surface KERNAUD.CPP calls, so that
   file compiles unchanged on both platforms; SDLAUDIO.CPP implements it
   over SDL2's audio device instead of DirectSound.

   Nothing streams here.  DirectSound needed Service() because a small
   ring buffer had to be refilled; SDL pulls straight from the converted
   sample we already hold, so Service() is a no-op.
   ======================================================================== */
#ifndef SDLAUDIO_HPP
#define SDLAUDIO_HPP

#include "../PLATFORM.H"

/* The layout matters: KA_ParseWave memcpy's the 16-byte PCMWAVEFORMAT
   straight out of the RIFF chunk into the first 16 bytes of this. */
#if !defined(WAVE_FORMAT_PCM)
#define WAVE_FORMAT_PCM		1
#endif

typedef struct
{
	WORD	wFormatTag;			/* offset  0 */
	WORD	nChannels;			/*         2 */
	DWORD	nSamplesPerSec;		/*         4 */
	DWORD	nAvgBytesPerSec;	/*         8 */
	WORD	nBlockAlign;		/*        12 */
	WORD	wBitsPerSample;		/*        14 */
	WORD	cbSize;				/*        16 */
} WAVEFORMATEX;

class DSound
{
public:
	DSound();
	~DSound();

	/* buffSize is DirectSound's ring size and is ignored: the sample is
	   converted to the device format once and mixed from memory. */
	BOOL	Init(DWORD buffSize, LPSTR sampPtr, DWORD sampSize,
	             WORD bits, DWORD rate, WORD channels);
	BOOL	Play(LPSTR sampPtr);
	BOOL	Stop(void);
	BOOL	IsPlaying(void);
	BOOL	Service(LPSTR sampPtr);
	void	SetLoop(BOOL loop);
	BOOL	SetVolume(WORD vol, WORD factor);		/* vol 0..100 */
	BOOL	SetPan(WORD panPercent, WORD factor);	/* 0 left .. 100 right */

	/* Touched only by the mixer callback, under the device lock. */
	short *	fpcm;			/* device-format frames, or NULL */
	DWORD	fFrames;		/* length in frames */
	DWORD	fPos;			/* playback cursor, in frames */
	BOOL	fPlaying;
	BOOL	fLoop;
	float	fGainL;
	float	fGainR;

private:
	WORD	fVol;			/* 0..100 */
	WORD	fPan;			/* 0..100, 50 = centre */
	void	mfRecomputeGains(void);
};

BOOL	DetectDSound(void);
BOOL	InitDSound(HWND hwnd);
void	TermDSound(void);

/* DirectSound could be asked to reformat its primary buffer. SDL fixes the
   device format at open time, so this just reports what was opened. */
void	SetPrimaryFormat(DWORD * pRate, WORD * pBits, WORD * pChannels);

#endif /* SDLAUDIO_HPP */
