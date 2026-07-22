/* ========================================================================
   KERNAUD.CPP - Audio kernel over the SOLAUDIO DirectSound backend.

   Reconstruction of the lost Sierra audio-kernel layer.  The original
   (WAUDIO/AudioMgrDir) implemented a large SCI-engine interface; the
   game only ever used the small surface declared in KERNAUD.HPP, so
   this implementation drives SOLAUDIO's DSound sample class directly.

   Sample data comes from, in order:
     1. <id>.WAV in the working directory (voice scratch, e.g. 499.wav)
     2. MUSIC\<id>.WAV                    (digital music tracks - streamed)
     3. <id>.WAV via the resource manager (effects in wave.res - the
        resource handle is kept and the pointer re-fetched every use, so
        purge/reload and compaction by the memory manager are harmless)
   ======================================================================== */
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SYSTEM.H"				// resource manager, BLKPTR, fERROR
#include "SOLAUDIO\dsound.hpp"
#include "KERNAUD.HPP"

extern HWND hwndApp;			// WINSYS application window

#define KA_SAMPLES		8			// simultaneous samples (SOLAUDIO MAXSAMPLES)
#define KA_STATIC_MAX	(256L * 1024)	// bigger samples stream; smaller play static

typedef struct {
	int		id;					// wave number, e.g. 1001 -> "1001.WAV"
	int		tag;				// caller's instance cookie
	int		vol;				// last requested volume, 0..127
	BOOL	fInUse;
	BOOL	fStreaming;			// needs Service() pumping
	SHORT	hRes;				// resource handle, or fERROR if file-based
	UBYTE *	pFile;				// malloc'd file image, or NULL
	ULONG	cbFile;				// file image size (0 for resource-based)
	ULONG	oData;				// offset of PCM data in the WAV image
	ULONG	cbData;				// PCM data length
	DSound *pSnd;
} KA_SAMPLE;

static KA_SAMPLE	kaSample[KA_SAMPLES];
static BOOL			kaUp = FALSE;

static KernelAudio	kaKernel;
KernelAudio *		kernel = &kaKernel;

/* ------------------------------------------------------------------------
   Helpers
   ------------------------------------------------------------------------ */

// 0..127 game volume -> 0..100 DSound percent
static WORD KA_Pct (int volume)
{
	if (volume < 0)   volume = 0;
	if (volume > 127) volume = 127;
	return (WORD)((volume * 100) / 127);
}

// Current address of a sample's WAV image (resource blocks can move or be
// purged+reloaded between calls, so never cache the pointer).
static UBYTE * KA_Image (KA_SAMPLE * s)
{
	if (s->pFile != NULL)
		return s->pFile;
	return (UBYTE *)BLKPTR(s->hRes);
}

// Minimal RIFF/WAVE walk: find "fmt " and "data".
static BOOL KA_ParseWave (const UBYTE * img, ULONG cbImg,
	WAVEFORMATEX * pwfx, ULONG * poData, ULONG * pcbData)
{
	ULONG	pos;
	BOOL	fFmt = FALSE, fData = FALSE;

	if (cbImg < 44 ||
	    memcmp(img, "RIFF", 4) != 0 ||
	    memcmp(img + 8, "WAVE", 4) != 0)
	{
		return FALSE;
	}

	// Trust the RIFF length if the caller couldn't size the buffer.
	{
		ULONG cbRiff = *(ULONG *)(img + 4) + 8;
		if (cbImg == 0 || cbRiff < cbImg)
			cbImg = cbRiff;
	}

	memset(pwfx, 0, sizeof(*pwfx));

	for (pos = 12; pos + 8 <= cbImg && !(fFmt && fData); )
	{
		ULONG cbChunk = *(ULONG *)(img + pos + 4);

		if (memcmp(img + pos, "fmt ", 4) == 0 && cbChunk >= 16)
		{
			memcpy(pwfx, img + pos + 8, 16);	// PCMWAVEFORMAT part
			pwfx->cbSize = 0;
			fFmt = TRUE;
		}
		else if (memcmp(img + pos, "data", 4) == 0)
		{
			*poData  = pos + 8;
			*pcbData = cbChunk;
			if (*poData + *pcbData > cbImg)
				*pcbData = cbImg - *poData;
			fData = TRUE;
		}
		pos += 8 + ((cbChunk + 1) & ~1UL);		// chunks are word-aligned
	}

	return fFmt && fData && pwfx->wFormatTag == WAVE_FORMAT_PCM && *pcbData > 0;
}

// Locate the WAV image for <id>: loose file first, then the resource manager.
static BOOL KA_Acquire (KA_SAMPLE * s, int id)
{
	static const char * apszDirs[] = { "", "MUSIC\\" };
	char	szName[64];
	int		i;

	s->hRes  = (SHORT)fERROR;
	s->pFile = NULL;
	s->cbFile = 0;

	for (i = 0; i < 2; i++)
	{
		FILE *	f;
		long	cb;

		sprintf(szName, "%s%d.WAV", apszDirs[i], id);
		f = fopen(szName, "rb");
		if (f == NULL)
			continue;

		fseek(f, 0, SEEK_END);
		cb = ftell(f);
		fseek(f, 0, SEEK_SET);
		if (cb > 44)
		{
			s->pFile = (UBYTE *)malloc(cb);
			if (s->pFile != NULL && fread(s->pFile, 1, cb, f) == (size_t)cb)
			{
				s->cbFile = cb;
				fclose(f);
				return TRUE;
			}
			free(s->pFile);
			s->pFile = NULL;
		}
		fclose(f);
	}

	sprintf(szName, "%d.WAV", id);
	s->hRes = GetResourceStd(szName, FALSE);
	return s->hRes >= 0;
}

static void KA_Release (KA_SAMPLE * s)
{
	if (s->pSnd != NULL)
	{
		s->pSnd->Stop();
		delete s->pSnd;
		s->pSnd = NULL;
	}
	if (s->pFile != NULL)
	{
		free(s->pFile);
		s->pFile = NULL;
	}
	// Resource-based data stays cached in the resource manager.
	s->hRes   = (SHORT)fERROR;
	s->fInUse = FALSE;
}

// Free every slot whose sample finished playing.
static void KA_Reap (void)
{
	int i;
	for (i = 0; i < KA_SAMPLES; i++)
	{
		if (kaSample[i].fInUse && !kaSample[i].pSnd->IsPlaying())
			KA_Release(&kaSample[i]);
	}
}

/* ------------------------------------------------------------------------
   Kernel lifetime
   ------------------------------------------------------------------------ */

int KernelAudioInitialize (void)
{
	DWORD	rate = 22050;
	WORD	bits = 16, channels = 2;

	if (kaUp)
		return TRUE;
	if (!DetectDSound() || !InitDSound(hwndApp))
		return FALSE;

	SetPrimaryFormat(&rate, &bits, &channels);	// best effort

	memset(kaSample, 0, sizeof(kaSample));
	kaUp = TRUE;
	return TRUE;
}

void KernelAudioTerminate (void)
{
	int i;

	if (!kaUp)
		return;
	for (i = 0; i < KA_SAMPLES; i++)
	{
		if (kaSample[i].fInUse)
			KA_Release(&kaSample[i]);
	}
	TermDSound();
	kaUp = FALSE;
}

void KernelAudioService (void)
{
	int i;

	if (!kaUp)
		return;
	for (i = 0; i < KA_SAMPLES; i++)
	{
		KA_SAMPLE * s = &kaSample[i];
		if (!s->fInUse)
			continue;
		if (s->fStreaming)
			s->pSnd->Service((LPSTR)(KA_Image(s) + s->oData));
		if (!s->pSnd->IsPlaying())
			KA_Release(s);
	}
}

/* ------------------------------------------------------------------------
   KernelAudio methods
   ------------------------------------------------------------------------ */

int KernelAudio::AudioPlay (int id, int fLoop, int volume, int tag, long /*fPause*/)
{
	KA_SAMPLE *		s = NULL;
	WAVEFORMATEX	wfx;
	UBYTE *			img;
	ULONG			cbImg;
	DWORD			cbBuff;
	int				i;

	if (!kaUp)
		return 0;

	KA_Reap();
	for (i = 0; i < KA_SAMPLES; i++)
	{
		if (!kaSample[i].fInUse)
		{
			s = &kaSample[i];
			break;
		}
	}
	if (s == NULL)
		return 0;						// all channels busy

	memset(s, 0, sizeof(*s));
	s->hRes = (SHORT)fERROR;
	if (!KA_Acquire(s, id))
		return 0;

	img   = KA_Image(s);
	cbImg = s->cbFile;					// 0 for resource: RIFF length is used
	if (img == NULL ||
	    !KA_ParseWave(img, cbImg ? cbImg : 0xFFFFFFF0UL, &wfx, &s->oData, &s->cbData))
	{
		KA_Release(s);
		return 0;
	}

	// Small effects play from a static DirectSound buffer; long samples
	// (music tracks) stream and get pumped by KernelAudioService.
	cbBuff = (s->cbData <= KA_STATIC_MAX) ? 0 : wfx.nAvgBytesPerSec;
	s->fStreaming = (cbBuff != 0);

	s->pSnd = new DSound;
	if (!s->pSnd->Init(cbBuff, (LPSTR)(img + s->oData), s->cbData,
	                   wfx.wBitsPerSample, wfx.nSamplesPerSec, wfx.nChannels))
	{
		KA_Release(s);
		return 0;
	}

	s->pSnd->SetLoop(fLoop);
	s->pSnd->SetVolume(KA_Pct(volume), 1);

	if (!s->pSnd->Play((LPSTR)(KA_Image(s) + s->oData)))
	{
		KA_Release(s);
		return 0;
	}

	s->id     = id;
	s->tag    = tag;
	s->vol    = volume;
	s->fInUse = TRUE;
	return 1;
}

void KernelAudio::AudioStop (int id, int tag)
{
	int i;
	for (i = 0; i < KA_SAMPLES; i++)
	{
		KA_SAMPLE * s = &kaSample[i];
		if (s->fInUse && s->id == id && (tag == 0 || s->tag == tag))
			KA_Release(s);
	}
}

void KernelAudio::AudioStopAll (void)
{
	int i;
	for (i = 0; i < KA_SAMPLES; i++)
	{
		if (kaSample[i].fInUse)
			KA_Release(&kaSample[i]);
	}
}

void KernelAudio::AudioVolume (int id, int volume, int tag)
{
	int i;
	for (i = 0; i < KA_SAMPLES; i++)
	{
		KA_SAMPLE * s = &kaSample[i];
		if (s->fInUse && s->id == id && (tag == 0 || s->tag == tag))
		{
			s->vol = volume;
			s->pSnd->SetVolume(KA_Pct(volume), 1);
		}
	}
}

void KernelAudio::AudioVolumeAll (int volume)
{
	int i;
	for (i = 0; i < KA_SAMPLES; i++)
	{
		if (kaSample[i].fInUse)
		{
			kaSample[i].vol = volume;
			kaSample[i].pSnd->SetVolume(KA_Pct(volume), 1);
		}
	}
}

void KernelAudio::AudioPan (int id, int panPercent, int tag)
{
	int i;

	if (panPercent < 0)   panPercent = 0;
	if (panPercent > 100) panPercent = 100;

	for (i = 0; i < KA_SAMPLES; i++)
	{
		KA_SAMPLE * s = &kaSample[i];
		if (s->fInUse && s->id == id && (tag == 0 || s->tag == tag))
			s->pSnd->SetPan((WORD)panPercent, 1);
	}
}

int KernelAudio::AudioSampleVolume (int id, int * pVolume, int tag)
{
	int i;
	for (i = 0; i < KA_SAMPLES; i++)
	{
		KA_SAMPLE * s = &kaSample[i];
		if (s->fInUse && s->id == id && (tag == 0 || s->tag == tag) &&
		    s->pSnd->IsPlaying())
		{
			if (pVolume != NULL)
				*pVolume = s->vol;
			return 1;
		}
	}
	return 0;
}

int KernelAudio::AudioActiveSamples (void)
{
	int i, c = 0;

	if (!kaUp)
		return 0;
	KA_Reap();
	for (i = 0; i < KA_SAMPLES; i++)
	{
		if (kaSample[i].fInUse)
			c++;
	}
	return c;
}

// The DirectSound backend mixes at the primary-buffer format natively;
// these knobs existed for the DOS DAC driver and are accepted as no-ops.
void KernelAudio::AudioSetVolReduction (int)	{}
void KernelAudio::AudioSetChannels (int)		{}
void KernelAudio::AudioSetRate (int)			{}
void KernelAudio::AudioDefaultBits (void)		{}
