/* ========================================================================
   SDLAUDIO.CPP - DSound-on-SDL2, the POSIX audio backend.

   One DSound object is one voice.  Samples are converted to the device
   format once at Init and mixed in the SDL callback; the game's own copy
   of the wave may be a resource block that moves or gets purged, so the
   audio thread must never point into it.

   The device runs at 22050 Hz because every shipped asset is 22050 or
   below - music is 22050/mono/16-bit - so nothing is ever upsampled.
   ======================================================================== */
#include "SDLAUDIO.HPP"

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>

#define KA_DEV_RATE		22050
#define KA_DEV_CHANNELS	2
#define KA_DEV_FRAMES	1024		/* ~46 ms of latency */
#define KA_MAX_VOICES	16			/* KERNAUD uses 8; headroom is free */

static SDL_AudioDeviceID	gDev = 0;
static DSound *				gVoice[KA_MAX_VOICES];

/* ------------------------------------------------------------------------
   Mixer
   ------------------------------------------------------------------------ */

/* Accumulate in 32 bits and clamp once at the end: clamping per voice
   would distort whenever two loud samples overlap. */
static void KA_Callback(void * /*unused*/, Uint8 * stream, int len)
{
	static int	acc[KA_DEV_FRAMES * KA_DEV_CHANNELS];
	short *		out = (short *) stream;
	int			samples = len / (int) sizeof(short);
	int			frames = samples / KA_DEV_CHANNELS;
	int			i, v;

	if (frames > KA_DEV_FRAMES)
		frames = KA_DEV_FRAMES;			/* SDL never asks for more */
	samples = frames * KA_DEV_CHANNELS;

	memset(stream, 0, (size_t) len);
	memset(acc, 0, (size_t) samples * sizeof(acc[0]));

	for (v = 0; v < KA_MAX_VOICES; v++)
	{
		DSound * s = gVoice[v];

		if (s == NULL || !s->fPlaying || s->fpcm == NULL)
			continue;

		for (i = 0; i < frames; i++)
		{
			const short * src;

			if (s->fPos >= s->fFrames)
			{
				if (!s->fLoop)
				{
					s->fPlaying = FALSE;
					break;
				}
				s->fPos = 0;
			}

			src = s->fpcm + (s->fPos * KA_DEV_CHANNELS);
			acc[i * 2]     += (int) (src[0] * s->fGainL);
			acc[i * 2 + 1] += (int) (src[1] * s->fGainR);
			s->fPos++;
		}
	}

	for (i = 0; i < samples; i++)
	{
		int t = acc[i];

		if (t >  32767) t =  32767;
		if (t < -32768) t = -32768;
		out[i] = (short) t;
	}
}

/* ------------------------------------------------------------------------
   Device lifecycle
   ------------------------------------------------------------------------ */

BOOL DetectDSound(void)
{
	return (SDL_InitSubSystem(SDL_INIT_AUDIO) == 0) ? TRUE : FALSE;
}

BOOL InitDSound(HWND /*hwnd*/)
{
	SDL_AudioSpec want, got;

	if (gDev != 0)
		return TRUE;

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
	{
		printf("WARNING - SDL audio unavailable: %s\n", SDL_GetError());
		return FALSE;
	}

	memset(&want, 0, sizeof(want));
	want.freq     = KA_DEV_RATE;
	want.format   = AUDIO_S16SYS;
	want.channels = KA_DEV_CHANNELS;
	want.samples  = KA_DEV_FRAMES;
	want.callback = KA_Callback;

	/* No format changes: the mixer and every converted sample are built
	   for exactly this layout. */
	gDev = SDL_OpenAudioDevice(NULL, 0, &want, &got, 0);
	if (gDev == 0)
	{
		printf("WARNING - SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
		return FALSE;
	}

	memset(gVoice, 0, sizeof(gVoice));
	SDL_PauseAudioDevice(gDev, 0);
	printf("SDL audio: %d Hz, %d channels, %d-frame buffer\n",
	       got.freq, got.channels, got.samples);
	return TRUE;
}

void SetPrimaryFormat(DWORD * pRate, WORD * pBits, WORD * pChannels)
{
	if (pRate)     *pRate     = KA_DEV_RATE;
	if (pBits)     *pBits     = 16;
	if (pChannels) *pChannels = KA_DEV_CHANNELS;
}

void TermDSound(void)
{
	if (gDev != 0)
	{
		SDL_CloseAudioDevice(gDev);
		gDev = 0;
	}
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

/* ------------------------------------------------------------------------
   DSound - one voice
   ------------------------------------------------------------------------ */

DSound::DSound()
	: fpcm(NULL), fFrames(0), fPos(0), fPlaying(FALSE), fLoop(FALSE),
	  fGainL(1.0f), fGainR(1.0f), fVol(100), fPan(50)
{
	int i;

	if (gDev != 0)
		SDL_LockAudioDevice(gDev);

	for (i = 0; i < KA_MAX_VOICES; i++)
		if (gVoice[i] == NULL)
		{
			gVoice[i] = this;
			break;
		}

	if (gDev != 0)
		SDL_UnlockAudioDevice(gDev);
}

DSound::~DSound()
{
	int i;

	if (gDev != 0)
		SDL_LockAudioDevice(gDev);

	fPlaying = FALSE;
	for (i = 0; i < KA_MAX_VOICES; i++)
		if (gVoice[i] == this)
			gVoice[i] = NULL;

	if (gDev != 0)
		SDL_UnlockAudioDevice(gDev);

	/* Freed after unregistering, so the callback can no longer reach it. */
	free(fpcm);
	fpcm = NULL;
}

BOOL DSound::Init(DWORD /*buffSize*/, LPSTR sampPtr, DWORD sampSize,
                  WORD bits, DWORD rate, WORD channels)
{
	SDL_AudioCVT	cvt;
	SDL_AudioFormat	src = (bits == 8) ? AUDIO_U8 : AUDIO_S16LSB;

	if (sampPtr == NULL || sampSize == 0 || gDev == 0)
		return FALSE;

	if (SDL_BuildAudioCVT(&cvt, src, (Uint8) channels, (int) rate,
	                      AUDIO_S16SYS, KA_DEV_CHANNELS, KA_DEV_RATE) < 0)
	{
		printf("WARNING - unsupported wave format %d-bit %luHz %dch: %s\n",
		       (int) bits, (unsigned long) rate, (int) channels, SDL_GetError());
		return FALSE;
	}

	if (cvt.needed)
	{
		cvt.len = (int) sampSize;
		cvt.buf = (Uint8 *) malloc((size_t) cvt.len * cvt.len_mult);
		if (cvt.buf == NULL)
			return FALSE;

		memcpy(cvt.buf, sampPtr, (size_t) sampSize);
		if (SDL_ConvertAudio(&cvt) < 0)
		{
			free(cvt.buf);
			return FALSE;
		}
		fpcm    = (short *) cvt.buf;
		fFrames = (DWORD) cvt.len_cvt / (KA_DEV_CHANNELS * sizeof(short));
	}
	else
	{
		fpcm = (short *) malloc((size_t) sampSize);
		if (fpcm == NULL)
			return FALSE;
		memcpy(fpcm, sampPtr, (size_t) sampSize);
		fFrames = sampSize / (KA_DEV_CHANNELS * sizeof(short));
	}

	fPos = 0;
	return (fFrames > 0) ? TRUE : FALSE;
}

BOOL DSound::Play(LPSTR /*sampPtr*/)
{
	if (fpcm == NULL || gDev == 0)
		return FALSE;

	SDL_LockAudioDevice(gDev);
	fPos     = 0;
	fPlaying = TRUE;
	SDL_UnlockAudioDevice(gDev);
	return TRUE;
}

BOOL DSound::Stop(void)
{
	if (gDev != 0)
		SDL_LockAudioDevice(gDev);
	fPlaying = FALSE;
	if (gDev != 0)
		SDL_UnlockAudioDevice(gDev);
	return TRUE;
}

BOOL DSound::IsPlaying(void)
{
	return fPlaying;
}

/* DirectSound refilled a ring buffer here; nothing to do. */
BOOL DSound::Service(LPSTR /*sampPtr*/)
{
	return TRUE;
}

void DSound::SetLoop(BOOL loop)
{
	fLoop = loop;
}

/* ponytail: linear gain, not DirectSound's millibel curve. If quiet
   settings sound too loud, square it before reaching for a real dB map. */
void DSound::mfRecomputeGains(void)
{
	float gain = (float) fVol / 100.0f;
	float p    = (float) fPan / 100.0f;
	float l    = 2.0f * (1.0f - p);
	float r    = 2.0f * p;

	if (l > 1.0f) l = 1.0f;			/* centre must not halve the volume */
	if (r > 1.0f) r = 1.0f;

	fGainL = gain * l;
	fGainR = gain * r;
}

BOOL DSound::SetVolume(WORD vol, WORD /*factor*/)
{
	if (vol > 100)
		vol = 100;
	fVol = vol;
	mfRecomputeGains();
	return TRUE;
}

BOOL DSound::SetPan(WORD panPercent, WORD /*factor*/)
{
	if (panPercent > 100)
		panPercent = 100;
	fPan = panPercent;
	mfRecomputeGains();
	return TRUE;
}
