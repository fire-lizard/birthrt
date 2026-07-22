/* ========================================================================
   KERNAUD.HPP - Audio kernel interface.

   Reconstruction of the lost Sierra audio-kernel glue.  The original
   KERNAUD.HPP shipped with the SOLAUDIO library and was never recovered
   with this source tree; this version exposes exactly the surface the
   game uses (SOUND.CPP) and is implemented in KERNAUD.CPP directly on
   top of SOLAUDIO's DirectSound backend (SOLAUDIO/Dsound.cpp).

   Wave data resolution, in order:
     1. <id>.WAV in the working directory (voice scratch files, e.g. 499.wav)
     2. MUSIC\<id>.WAV                    (digital music tracks)
     3. <id>.WAV via the resource manager (sound effects in wave.res)
   ======================================================================== */
#ifndef KERNAUD_HPP
#define KERNAUD_HPP

class KernelAudio
{
public:
	// Play sample <id>; returns nonzero on success.  <volume> is 0..127,
	// <tag> is the caller's instance cookie for later Stop/Volume/Pan calls.
	int	AudioPlay (int id, int fLoop, int volume, int tag, long fPause);

	void	AudioStop (int id, int tag = 0);
	void	AudioStopAll ();

	void	AudioVolume (int id, int volume, int tag = 0);		// volume 0..127
	void	AudioVolumeAll (int volume);
	void	AudioPan (int id, int panPercent, int tag = 0);		// 0 left..100 right

	// Returns nonzero (and the sample's volume) while <id>/<tag> is playing.
	int	AudioSampleVolume (int id, int * pVolume, int tag = 0);

	int	AudioActiveSamples ();

	// Accepted for compatibility; the DirectSound backend mixes natively.
	void	AudioSetVolReduction (int fFlag);
	void	AudioSetChannels (int iChannels);
	void	AudioSetRate (int iRate);
	void	AudioDefaultBits ();
};

extern KernelAudio * kernel;

int	KernelAudioInitialize (void);
void	KernelAudioTerminate (void);
void	KernelAudioService (void);

#endif // KERNAUD_HPP
