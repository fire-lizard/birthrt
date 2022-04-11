// audio.hpp

#ifndef AUDIO_HPP
#define AUDIO_HPP

//#ifdef AUDIO_LIB

#include "custmsys.hpp"

//#else		// !AUDIO_LIB

//#ifndef RESTYPE_HPP
//#include "restype.hpp"
//#endif

//#endif	// #ifdef AUDIO_LIB

const unsigned MAXSAMPLES = 8;
const unsigned MAXVOLUME = 127;

// Audio Kernel Functions
enum	audioFuncs {
	WPLAY = 1,
	PLAY,
	STOP,
	PAUSE,
	RESUME,
	LOC,
	RATE,
	VOLUME,
	DACFOUND,
	BITS,
	DISTORT,
	DACMIXNORMAL,
	CHANNELS,
	PRELOAD,
	FADE,
	FADE36,
	NOISE,
	DACCRITICAL,
	LOOP,
	PAN
};

#define	SYNCMASK		0x80
#define	RAVEMASK		0x40
#define	SEQUMASK		0x3F

// (Note: DAC_HBUFFSIZE must be a 2K multiple if ARIA is supported)
#define	DAC_HBUFFSIZE	(1*1024)
#define	DAC_MAXHBSIZE	(16*1024)
#define	INT_HBUFFSIZE	(28*1024)
#define	INT_MAXHBSIZE	(31*1024)

const unsigned	SFXMODNUM = 65535U;

#define	ALL_SAMPLES	-1
#define	NO_SAMPLES	-2
#define	ROBOTS_ONLY	-3
#define	SFX_ONLY		-4
#define	NO_DAC		-1

// structure of information in resource map file 
// used for audio type resource only 
#ifndef VISUALCPP
struct ResAudEntry {
#else
#pragma pack(push, 1)
struct ResAudEntry {
#endif
	unsigned short	id;
	unsigned short	offsetLSW;
	unsigned char	offsetMSB;
};
#ifdef VISUALCPP
#pragma pack(pop)
#endif

// structure of information in resource map file 
// used for sync+rave+audio base-36 type resource only 
#ifndef VISUALCPP
struct ResAud36Entry {
#else
#pragma pack(push, 1)
struct ResAud36Entry {
#endif
	unsigned char	noun;
	unsigned char	verb;
	unsigned char	cond;
	union {
		unsigned char	sequ;
		unsigned char	sync;
		unsigned char	rave;
	} flag; 
	unsigned short	offsetLSW;
	unsigned char	offsetMSB;
	unsigned short	syncLen;
	unsigned short	raveLen;
};
#ifdef VISUALCPP
#pragma pack(pop)
#endif

#ifndef AUDIO_LIB
struct RobotAudStatus
{
	int	bytesPlayed;
	int	bytesPlaying;
	int	bytesSubmitted;
	int	rate;
	int	bits;
};

struct RobotAudInfo
{
	RobotAudInfo(MemID buffID, int byteLen, int fileLoc)	:
		adrID(buffID), len(byteLen), floc(fileLoc)  {}

	RobotAudInfo() {}

	MemID		adrID;		// compressed buffer address
	int		len;			// compressed buffer length
	int		floc;			// original unc file-rel offset
	int		ext;			// original unc file-rel extent
	int		hwm[2];		// highest unc file-rel offset buffered
	int		min;			// lowest unc file-rel offset that can be buffered
	int		max;			// highest unc file-rel offset that can be buffered
	MemID		memID;		// uncompressed buffer
	int		blen;			// uncompressed buffer length
	int		channel;		// even or odd buffer
	int		cofs;			// uncompressed buffer offset
};

#endif  //  #ifndef AUDIO_LIB

extern Boolean	audioInstalled;
extern Boolean dontPollDAC;
extern int		audVolFD;
extern int		sfxVolFD;
extern int		activeSamples;


class AudioMgr {

public:
	virtual				~AudioMgr() {}

#ifndef	AUDIO_LIB
	virtual	Boolean	AudioVMDStart(MemID,int,int,int) = 0;
	virtual	uint	 	AudioVMDQuery() = 0;
	virtual	Boolean	AudioVMDStop() = 0;
	virtual	Boolean	AudRobot(RobotAudInfo) = 0;
	virtual	Boolean	QueryAudRobot(RobotAudStatus*) = 0;
	virtual	Boolean	AudioFree(int) = 0;
	virtual	Boolean	AudioLock(ResNum,Boolean) = 0;
	virtual	Boolean	AudioPurge(ResNum) = 0;
	virtual	Boolean	AudioQueryDiscardable(ResNum) = 0;
	virtual	int	 	FindAudEntry(ResNum) = 0;
	virtual	int	 	FindAud36Entry(ResNum,uchar,uchar,uchar,uchar) = 0;
	virtual	void	 	InitAudioVols() = 0;
	virtual	void	 	MakeName36(MemType,char*,ResNum,uchar,uchar,uchar,uchar) = 0;
#ifdef DEBUG
	virtual	void	 	CheckDiscStreaming(char*) = 0;
#endif
#endif

	virtual	Boolean	AudioResume(int) = 0;
	virtual	Boolean	AudioPause(int) = 0;
	virtual	void	 	AudioStop(int) = 0;
	virtual	void	 	EndAudio() = 0;
	virtual	Boolean	InitAudioDriver() = 0;
	virtual	void	 	SubmitMaxBuffers() = 0;
	virtual	void		DACCallBack(int,int) = 0;
	virtual	void		AudioServer() = 0;

	virtual	Boolean	ImpInitializeAudioDriver() = 0;
	virtual	void		ImpTerminateAudioDriver() = 0;
	virtual	Boolean	ImpAudioInstalled() = 0;
	virtual	int	 	ImpAudioPreload(int iValue=-1) = 0;
	virtual	int	 	ImpAudioActiveSamples() = 0;
	virtual	int	 	ImpAudioWaitToPlay(int iResNum, Boolean fLoop=False,
										int iVolume=MAXVOLUME, int iTag=0) = 0;
	virtual	int	 	ImpAudioWaitToPlay(int iModule, int iNoun, int iVerb, int iCond,
										int iSequ, Boolean fLoop=False, int iVolume=MAXVOLUME) = 0;
	virtual	int	 	ImpAudioPlay(int iResNum, Boolean fLoop=False, int iVolume=MAXVOLUME, int iTag=0, 
								  long fPause=False) = 0;
	virtual	int	 	ImpAudioPlay(int iModule, int iNoun, int iVerb, int iCond, int iSequ,
						  Boolean fLoop=False, int iVolume=MAXVOLUME, Boolean fPause=False) = 0;
	virtual	void	 	ImpAudioStopAll() = 0;
	virtual	void	 	ImpAudioStop(int iResNum, int iTag=0) = 0;
	virtual	void	 	ImpAudioStop(int iModule, int iNoun, int iVerb, int iCond, int iSequ) = 0;
	virtual	void	 	ImpAudioLoop(int iResNum, Boolean fLoop, int iTag=0) = 0;
	virtual	void	 	ImpAudioLoop(int iModule, int iNoun, int iVerb, int iCond, 
							int iSequ, Boolean fLoop) = 0;
	virtual	Boolean	ImpAudioPauseAll() = 0;
	virtual	Boolean	ImpAudioPause(int iResNum, int iTag=0) = 0;
	virtual	Boolean	ImpAudioPause(int iModule, int iNoun, int iVerb, int iCond, int iSequ) = 0;
	virtual	Boolean	ImpAudioResumeAll() = 0;
	virtual	Boolean	ImpAudioResume(int iResNum, int iTag=0) = 0;
	virtual	Boolean	ImpAudioResume(int iModule, int iNoun, int iVerb, int iCond, int iSequ) = 0;
	virtual	Boolean	ImpAudioDurationAll(int* iDuration) = 0;
	virtual	Boolean	ImpAudioDuration(int iResNum, int* iDuration, int iTag=0) = 0;
	virtual	Boolean	ImpAudioDuration(int iModule, int iNoun, int iVerb, int iCond, int iSequ, 
									 int* iDuration) = 0;
	virtual	void	 	ImpAudioSetRate(int iRate) = 0;
	virtual	void	 	ImpAudioDefaultRate() = 0;
	virtual	int	 	ImpAudioCurrentRate() = 0;
	virtual	void	 	ImpAudioSetBits(int iBits) = 0;
	virtual	void	 	ImpAudioDefaultBits() = 0;
	virtual	int	 	ImpAudioCurrentBits() = 0;
	virtual	void	 	ImpAudioSetChannels(int iChannels) = 0;
	virtual	void	 	ImpAudioDefaultChannels() = 0;
	virtual	int	 	ImpAudioCurrentChannels() = 0;
	virtual	Boolean	ImpAudioGetVolReduction() = 0;
	virtual	void	 	ImpAudioSetVolReduction(Boolean fFlag) = 0;
	virtual	Boolean	ImpAudioGetDACCritical() = 0;
	virtual	void	 	ImpAudioSetDACCritical(Boolean fFlag) = 0;
	virtual	void	 	ImpAudioDistortAll(int iDistortionMask) = 0;
	virtual	void	 	ImpAudioDistort(int iResNum, int iDistortionMask, int iTag=0) = 0;
	virtual	void	 	ImpAudioDistort(int iModule, int iNoun, int iVerb, int iCond, int iSequ, 
								int iDistortionMask) = 0;
	virtual	int	 	ImpAudioGlobalVolume() = 0;
	virtual	Boolean	ImpAudioSampleVolume(int iResNum, int* iVolume, int iTag=0) = 0;
	virtual	Boolean	ImpAudioSampleVolume(int iModule, int iNoun, int iVerb, int iCond, int iSequ,
										  int* iVolume) = 0;
	virtual	void	 	ImpAudioVolumeAll(int iVolume) = 0;
	virtual	void	 	ImpAudioVolume(int iResNum, int iVolume, int iTag=0) = 0;
	virtual	void	 	ImpAudioVolume(int iModule, int iNoun, int iVerb, int iCond, int iSequ,
							  int iVolume) = 0;
	virtual	Boolean	ImpAudioFade(int iResNum, int iTargetVolume, int iTicks, int iSteps,
								Boolean fEnd=False, int iTag=0) = 0;
	virtual	Boolean	ImpAudioFade36(int iModule, int iNoun, int iVerb, int iCond, int iSequ,
								  int iTargetVolume=0, int iTicks=30, int iSteps=10, 
								  Boolean fEnd=True) = 0;
	virtual	Boolean	ImpAudioCheckNoise() = 0;
	virtual	void	 	ImpAudioPan(int iResNum, int iPanPercent, int iTag=0) = 0;
	virtual	void	 	ImpAudioPan(int iModule, int iNoun, int iVerb, int iCond, int iSequ,
						  int iPanPercent) = 0;
	virtual	void	 	ImpAudioPanOff(int iResNum, int iTag=0) = 0;
	virtual	void	 	ImpAudioPanOff(int iModule, int iNoun, int iVerb, int iCond, int iSequ) = 0;
	virtual	int	 	ImpAudioSampNotInMem(void) = 0;
#ifdef WINDOWS
#ifndef AUDIO_LIB
	virtual	uint		AudioVMDPosLoc() = 0;
#endif
	virtual	void		PollWaveBuffs() = 0;;
	virtual	void		PauseWaveDev(void) = 0;
	virtual	void		ResumeWaveDev(void) = 0;
#endif
};

extern	AudioMgr	*	audioMgr;


#endif

