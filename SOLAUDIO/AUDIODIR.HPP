// audiodir.hpp

#ifndef AUDIODIR_HPP
#define AUDIODIR_HPP

#ifndef DSOUND_HPP
#include	"dsound.hpp"
#endif


class AudioMgrDir : public AudioMgr {

public:
	AudioMgrDir();
	~AudioMgrDir();

#ifndef AUDIO_LIB
	Boolean	AudioVMDStart(MemID,int,int,int,int vmdVolume=MAXVOLUME);
	uint	 	AudioVMDQuery();
	Boolean	AudioVMDStop();
	Boolean	AudRobot(RobotAudInfo);
	Boolean	QueryAudRobot(RobotAudStatus*);
	Boolean	AudioFree(int);
	Boolean	AudioLock(ResNum,Boolean);
	Boolean	AudioPurge(ResNum);
	Boolean	AudioQueryDiscardable(ResNum);
	int	 	FindAudEntry(ResNum);
	int	 	FindAud36Entry(ResNum,uchar,uchar,uchar,uchar);
	void	 	InitAudioVols();
	void	 	MakeName36(MemType,char*,ResNum,uchar,uchar,uchar,uchar);
	void		GetConvBuffer(MemID&,int);
#ifdef DEBUG
	void	 	CheckDiscStreaming(char*);
#endif
#endif
	Boolean	AudioPause(int);
	Boolean	AudioResume(int);
	void	 	SubmitMaxBuffers();
	void		DACCallBack(int,int);
	void		AudioServer();
	void	 	AudioStop(int);
	void	 	EndAudio();
	Boolean	InitAudioDriver();

	Boolean	ImpInitializeAudioDriver();
	void		ImpTerminateAudioDriver();
	Boolean	ImpAudioInstalled();
	int	 	ImpAudioPreload(int iValue=-1);
	int	 	ImpAudioActiveSamples();
	int	 	ImpAudioWaitToPlay(int iResNum, Boolean fLoop=False,
										int iVolume=MAXVOLUME, int iTag=0);   
	int	 	ImpAudioWaitToPlay(int iModule, int iNoun, int iVerb, int iCond,
										int iSequ, Boolean fLoop=False, int iVolume=MAXVOLUME);
	int	 	ImpAudioPlay(int iResNum, Boolean fLoop=False, int iVolume=MAXVOLUME, int iTag=0, 
								  long fPause=False);
	int	 	ImpAudioPlay(int iModule, int iNoun, int iVerb, int iCond, int iSequ,
						  Boolean fLoop=False, int iVolume=MAXVOLUME, Boolean fPause=False); 
	void	 	ImpAudioStopAll();
	void	 	ImpAudioStop(int iResNum, int iTag=0);
	void	 	ImpAudioStop(int iModule, int iNoun, int iVerb, int iCond, int iSequ);
	void	 	ImpAudioLoop(int iResNum, Boolean fLoop, int iTag=0);   
	void	 	ImpAudioLoop(int iModule, int iNoun, int iVerb, int iCond, 
							int iSequ, Boolean fLoop);   
	Boolean	ImpAudioPauseAll();   
	Boolean	ImpAudioPause(int iResNum, int iTag=0);
	Boolean	ImpAudioPause(int iModule, int iNoun, int iVerb, int iCond, int iSequ);
	Boolean	ImpAudioResumeAll();   
	Boolean	ImpAudioResume(int iResNum, int iTag=0);
	Boolean	ImpAudioResume(int iModule, int iNoun, int iVerb, int iCond, int iSequ);
	Boolean	ImpAudioDurationAll(int* iDuration);   
	Boolean	ImpAudioDuration(int iResNum, int* iDuration, int iTag=0);
	Boolean	ImpAudioDuration(int iModule, int iNoun, int iVerb, int iCond, int iSequ, 
									 int* iDuration);
	void	 	ImpAudioSetRate(int iRate);
	void	 	ImpAudioDefaultRate();
	int	 	ImpAudioCurrentRate();
	void	 	ImpAudioSetBits(int iBits);
	void	 	ImpAudioDefaultBits();
	int	 	ImpAudioCurrentBits();
	void	 	ImpAudioSetChannels(int iChannels);
	void	 	ImpAudioDefaultChannels();
	int	 	ImpAudioCurrentChannels();
	Boolean	ImpAudioGetVolReduction();
	void	 	ImpAudioSetVolReduction(Boolean fFlag);
	Boolean	ImpAudioGetDACCritical();
	void	 	ImpAudioSetDACCritical(Boolean fFlag);
	void	 	ImpAudioDistortAll(int iDistortionMask);   
	void	 	ImpAudioDistort(int iResNum, int iDistortionMask, int iTag=0);   
	void	 	ImpAudioDistort(int iModule, int iNoun, int iVerb, int iCond, int iSequ, 
								int iDistortionMask);
	int	 	ImpAudioGlobalVolume();
	Boolean	ImpAudioSampleVolume(int iResNum, int* iVolume, int iTag=0);   
	Boolean	ImpAudioSampleVolume(int iModule, int iNoun, int iVerb, int iCond, int iSequ,
										  int* iVolume);
	void	 	ImpAudioVolumeAll(int iVolume);   
	void	 	ImpAudioVolume(int iResNum, int iVolume, int iTag=0);   
	void	 	ImpAudioVolume(int iModule, int iNoun, int iVerb, int iCond, int iSequ,
							  int iVolume);   
	Boolean	ImpAudioFade(int iResNum, int iTargetVolume, int iTicks, int iSteps,
								Boolean fEnd=False, int iTag=0);
	Boolean	ImpAudioFade36(int iModule, int iNoun, int iVerb, int iCond, int iSequ,
								  int iTargetVolume=0, int iTicks=30, int iSteps=10, 
								  Boolean fEnd=True);
	Boolean	ImpAudioCheckNoise();
	void	 	ImpAudioPan(int iResNum, int iPanPercent, int iTag=0);   
	void	 	ImpAudioPan(int iModule, int iNoun, int iVerb, int iCond, int iSequ, 
						  int iPanPercent);
	void	 	ImpAudioPanOff(int iResNum, int iTag=0);   
	void	 	ImpAudioPanOff(int iModule, int iNoun, int iVerb, int iCond, int iSequ);
	int	 	ImpAudioSampNotInMem(void);
	uint		AudioVMDPosLoc();
	void		PollWaveBuffs();
	void		PauseWaveDev(void);
	void		ResumeWaveDev(void);
	void		DumpAudio(void);
	void		DumpMemID(MemID memID, char *name, int dumpCount, int n);
	void		EnterCritical(void);
	void		LeaveCritical(void);
	void		ServiceAud(void);

private:
	DSound	*CreateDSoundObject(void);
	int		AudioSelect(int n,ResNum module,int num,uchar noun,uchar verb,
				uchar cond,uchar sequ,int tag,Boolean loop);
	void		AudioPlay(int n);
	void		StartTimer(void);
	void		StopTimer(void);
	void		DropSample(int n);
	int		AudioLoc(int n);
	void		AudioDistort(int request, int n);
	void		AudioVolume(int request, int n);
	void		AudioPan(int n, int panPercent);
	WORD		ScaleVolume(int n);
	void		SetBuffSize(int n);
	DWORD		GetMSec(int n, DWORD buffSize);
	void		InitCompressionValues(int n);
	void		PrimeFeedBuff(int n);
	void		FillBuffer(int n, DWORD fillPos, DWORD fillLen);
	void		MoveData(int n, DWORD writePos, DWORD writeLen);
	void		GetSomeData(int n, DWORD writePos, DWORD writeLen);
	void		Decompress(int n, DWORD writePos, DWORD writeLen);
	void		GetData(int n, DWORD writePos, DWORD writeLen);
	void		FillSilence(int n, DWORD fillPos, DWORD fillLen);
	Boolean	FindSample(int n, char *pathName);
	void		ConvBase36(char *str, int num10, int digits);
	char		GetDigit36(int n);
	int		CheckNoise();
	Boolean	CheckSOLFormat(int n);
	Boolean	CheckWAVEFormat(int n);
	Boolean	GetSampleNumber(int iResNum, int* iSampleNumber, int iTag);
	Boolean	GetSampleNumber(int iModule, int iNoun, int iVerb, int iCond, 
										int iSequ, int* iSampleNumber);
	int		WAudioPlay(Boolean licensePlate, int iResNum, int iTag, int iModule, int iNoun,
			int iVerb, int iCond, int iSequ, Boolean fLoop, int iVolume,
			Boolean fPause);
	void		RobotPlay(int n);
#ifndef AUDIO_LIB
	void		FillRbotBuffer(int n);
	void		FillRData(int n, int srStart, int srEnd, int type);
	void		MoveRData(int n, int brSrcPos, int brDestPos, int lenBytes, int type);
#endif
	void		ThreadAssertFail(int n, char* file, int line, char* expression);
	void		ThreadAlert(char *message);
	int		GetActiveSamples();
	void		MarkStopped(int n);

	struct Sample {
		int		attrs;
		uchar		bits;
		BOOL		bufferPrimed;
		uchar		channels;
		int		compressed;
		int		compValL;
		int		compValR;
		uchar		cond;
		int		distortion;
		DSound	*dSnd;			// pointer to direct sound object
		DWORD		dSoundBuffSize;// size of direct sound buffer
		int		fadeEndVol;		// final value of volume (0-MAXVOLUME)
		int		fadeStartTime;	// system time at start of fade - 0 if no fade
		int		fadeStartVol;	// starting value of volume (0-MAXVOLUME)
		BOOL		fadeStop;		// TRUE if Stop sample when fade finished
		int		fadeTotTime; 	// duration of fade (msec)
		int		fd;
		DWORD		feedBuffPos;	// next position in fill buff to be written
		DWORD		feedBuffSize;	// size of feed buffer
		MemID		feedID;			// feed buffer
		DWORD		fileBuffPos;	// next read position in file buffer
		DWORD		fileBuffSize;	// size of file i/o buffer
		MemID		fileID;			// file i/o buffer
		int		fileOfs;
		DWORD		fileReadPos;	// next read position in file
		BOOL		fillFeedBuff;	// indicates if feed buffer needs continous filling
		ULONG		finishTime;		// system time sample will finish
		int		length;
		Boolean	loop;
		MemID		memID;
		ResNum	module;
		uchar		noun;
		BOOL		nowPlaying;
		int		num;
		int		pan;
		Boolean	patch;
		int		pausedLoc;
		MemID		playID;			// play buffer (memID or feedID)
		int		playMSec;		// milliseconds to play the sample
		int		rate;
		DWORD		readStart;
		MemType	resType;
		Boolean	robot;
		DWORD		sampReadPos;	// next read position in sample (memory or file)
		uchar		sequ;
		int		start;
		int		startLoc;
		BOOL		stopped;
		BOOL		streaming;
		int		tag;
		int		ticks;
		int		uncompLen;
		uchar		verb;
		Boolean	vmd;
		int		volume;
	};

	Sample sample[MAXSAMPLES+1];

	int		rbotReadPos;
	int		rbotSilenceHWM;
	MemID		convID;
	Boolean	critical;
	int		critSectionCount;
	Boolean	DACCritical;
	int		distortion;
	int		ffloc;
	Boolean	mixCheck;
	int		preLoad;
	int		prioritySample;
	int		rate;
	int		bits;
	int		channels;
	int		masterVolume;
	Boolean	robotsActive;
	Boolean	robotsPaused;
	int		vmdSync;
	int		vmdSample;
	int		volFactor;
	ULONG		startTime;
	Boolean	dontPollDAC;
	Boolean	audioInstalled;
	int		audVolFD;
	int		sfxVolFD;
	int		activeSamples;
	volatile	BOOL	serviceSentry;
	CRITICAL_SECTION	memCrit;
#ifndef AUDIO_LIB
	RobotAudInfo rbot;
#endif
};


#endif

