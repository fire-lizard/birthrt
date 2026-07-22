// dsound.hpp

#ifndef DSOUND_HPP
#define DSOUND_HPP

#include <windows.h>
#include <dsound.h>

/***************************************************************************

NOTE: To compile this module you must be pointing to the DirectX include
		directory and link to "dsound.lib"


Interface to the Microsoft Direct Sound functions
-------------------------------------------------
		This module provides a simplified interface for using Direct Sound.
	It manages all format conversion and mixing for playing simultaneous DAC
	samples.  Direct Sound takes advantage of sound card hardware
	capabilities when they are available.

		Basically, you create a DSound object for each sample which is to be
	played, and initialize it with parameters such as the sample address,
	size, and format (rate, bits, channels).  You may also set a loop property
	to cause the sample to be looped.  When you call the Play method of the
	object, all the details of playing it are taken care of.  However, you
	must make a call to the object's Service method frequently enough to
	assure continous play.  The required service frequency is a function of
	the buffer size which you specify.  The exception to this is if you 
	create a "static" sample which is small enough to be loaded entirely
	by Direct Sound.

		While the sample is playing, a number of controls are available such
	as SetVolume, SetPan, Pause, Resume, and Stop.  There are also status
	calls to determine the current play position and the number of bytes
	played.  When the sample is finished, you should delete the object.

		Before any sample is played, Direct Sound itself must be initialized,
	and it should be terminated prior to program termination.  There are also
	some calls to determine the device capabilities and to control the format
	of the primary playback buffer.

	The following specifies the details of all the available calls.  There
	are two groups: 1) general Direct Sound control calls, and 2) methods
	associated with the DSound class instance for individual samples.



Summary:

Control calls:

	DetectDSound		- detect presence of direct sound
	InitDSound			- initialize direct sound
	TermDSound			- terminate direct sound
	GetPrimaryFormat	- get format of primary playback buffer
	SetPrimaryFormat	- set format of primary playback buffer
	GetCaps				- get capabilities of direct sound device


DSound object methods:

	Init					- initialize DSound object
	SetLoop				- set loop property
	Play					- start playing the sample
	Service				- sample service routine
	Stop					- stop playing
	Pause					- pause the sample
	Resume				- resume after pause
	SetVolume			- set volume level
	SetPan				- set channel balance
	GetReadPos			- get next read position
	GetBytesRead		- get total bytes read
	GetBytesPlayed		- get total bytes played
	IsPlaying			- determines if sample is currently playing
	GetErrorMsg			- retrieve the text of last error message
	DumpBuffer			- dump the contents of secondary buffer to disk




--------------------------
Direct Sound control calls
--------------------------

----------------------------------------------------------------------------
BOOL	DetectDSound(void)

	To determine if Direct Sound is installed and useable on the system.
	Returns TRUE if available, FALSE otherwise.


----------------------------------------------------------------------------
BOOL	InitDSound(HWND hMyWnd);

	Initialize the Direct Sound system.  Must be called prior to playing any
	samples.

	hMyWnd
		Handle of window requesting access to direct sound.

	return:
		TRUE if successful.


----------------------------------------------------------------------------
BOOL	TermDSound(void);

	Terminate access to direct sound.  Should be called prior to program
	termination.

	return:
		TRUE if successful.


----------------------------------------------------------------------------
BOOL	GetPrimaryFormat(DWORD *rate, WORD *bits, WORD *channels);

	Returns the current format of the primary direct sound playback buffer.

	rate
		Address of where to store the playback rate.

	bits
		Address of where to store the bits per sample.

	channels
		Address of where to store the number of channels.

	return:
		TRUE if successful.


----------------------------------------------------------------------------
BOOL	SetPrimaryFormat(DWORD *rate, WORD *bits, WORD *channels);

	Sets the format of the primary direct sound playback buffer.  The format
	must be one that the device is capable of playing (see GetCaps below).

	rate
		Address of the playback rate.

	bits
		Address of the bits per sample.

	channels
		Address of the number of channels.

	return:
		TRUE if successful.  FALSE if device not capable.


----------------------------------------------------------------------------
BOOL	GetCaps(LPDSCAPS lpDsCaps);

	Get the direct sound device capabilities into a DSCAPS structure.

	lpDsCaps
		Address of a DSCAPS structure to be filled in.  See direct sound
		doco for details.

	return:
		TRUE if successful.


----------------------------------------------------------------------------

---------------------
DSound object methods
---------------------

----------------------------------------------------------------------------
BOOL
DSound::Init(DWORD	buffSize,
						LPSTR	sampPtr,
						DWORD	sampSize,
						WORD	bits,
						DWORD	rate,
						WORD	channels)

	Initialize the DSound object prior to playing it.

	buffSize
		Number of bytes which should be allocated for the secondary playback
		buffer.  The larger this buffer is, the less frequently the service
		routine must be called.  The current defaults are a buffer size
		sufficient to hold one second's worth of data, and a service rate of
		ten times per second.

		If this value is zero, then it means that the sample will be treated
		as a "static" sample.  That is, the secondary playback buffer will
		be the same size as the sample and will be loaded with the entire
		sample.  If hardware permits, it may be loaded onto the sound card's
		memory.  It will be played as is, with or without looping.  It does
		not require a periodic Service call to keep it playing.  The
		IsPlaying method may be called to determine when it has finished.

	sampPtr
		Address of the sample.

	sampSize
		Size in bytes of the sample.

	bits
		Number of bits per sample.

	rate
		Playback rate in samples per second.

	channels
		Number of channels (1-mono, 2-stereo).

	return:
		TRUE if successful.


----------------------------------------------------------------------------
void
DSound::SetLoop(BOOL loop)

	Set the loop property of the sample.  This may be changed while the
	sample is playing.

	loop
		TRUE to loop the sample, FALSE to not loop.


----------------------------------------------------------------------------
BOOL
DSound::Play(LPSTR sampPtr)

	Start playing the sample.  Play may be called repeatedly to restart the
	sample.

	sampPtr
		Address of the sample (since it may have moved since initializing).
		It should not be allowed to move while in this routine.

	return:
		TRUE if successful.


----------------------------------------------------------------------------
BOOL
DSound::Service(LPSTR sampPtr)

	Service the playback process.  This must be called frequently enough
	to maintain continuous playback.  It should not be allowed to move while
	in this routine.

	sampPtr
		Address of the sample.

	return:
		TRUE if successful.  If not successful, GetErrorMsg should be called
			to get the text of the error message.


----------------------------------------------------------------------------
BOOL
DSound::Stop(void)

	Stop playing the sample.

	return:
		TRUE if successful.


----------------------------------------------------------------------------
BOOL
DSound::Pause(void)

	Pause the sample.

	return:
		TRUE if successful.


----------------------------------------------------------------------------
BOOL
DSound::Resume(void)

	Resume playing after a pause.

	return:
		TRUE if successful.


----------------------------------------------------------------------------
BOOL
DSound::SetVolume(WORD vol)

	Set the sample's volume.  This may be called before or during playback.

	vol
		Volume level from 0 (silence) to 100 (full volume).

	return:
		TRUE if successful.


----------------------------------------------------------------------------
BOOL
DSound::SetPan(WORD panPercent)

	Set the relative of left and right channels.

	panPercent
		Percentage of volume to apply to the right channel.  Zero is full left,
		100 is full right, and 50 is centered.

	return:
		TRUE if successful.


----------------------------------------------------------------------------
DWORD
DSound::GetReadPos(void)

	Return the sample's byte index position of the byte which will be read
	next into the DSound buffer.  I.e. bytes prior to this have already been
	processed and may be overwritten.


----------------------------------------------------------------------------
DWORD
DSound::GetBytesRead(void)

	Return the total number of bytes which have been read so far.  This is
	useful for tracking looping buffers which must be back-filled.


----------------------------------------------------------------------------
DWORD
DSound::GetBytesPlayed(void)

	Return the total number of bytes which have been played so far.  This is
	always less than or equal to GetBytesRead and represents the current
	play position.


----------------------------------------------------------------------------
BOOL
DSound::IsPlaying(void)

	Returns TRUE if the sample is currently playing.


----------------------------------------------------------------------------
LPSTR
DSound::GetErrorMsg(void)

	Returns a pointer to the error message text buffer.  This should be called
	if the service routine returns FALSE indicating an error occured.
	This is required because if the service routine is called from within a
	timer callback thread, error messages don't display correctly and so
	must be displayed later from outside the thread.


----------------------------------------------------------------------------
void
DSound::DumpBuffer(int dumpCount, int n)
	
	Dump the contents of the secondary buffer to disk file disk file
	"dsound.<dumpCount><n>".  Used for debugging.



****************************************************************************
*/



BOOL	DetectDSound(void);
BOOL	InitDSound(HWND);
BOOL	TermDSound(void);
BOOL	GetPrimaryFormat(DWORD *rate, WORD *bits, WORD *channels);
BOOL	SetPrimaryFormat(DWORD *rate, WORD *bits, WORD *channels);
BOOL	GetCaps(LPDSCAPS lpDsCaps);


class DSound
{
	public:
	DSound();
	~DSound();
	BOOL		Init(DWORD buffSize,LPSTR sampPtr,DWORD sampSize,WORD bits,DWORD rate,WORD channels);
	BOOL		Play(LPSTR sampPtr);
	BOOL		IsPlaying(void);
	DWORD		GetReadPos(void);
	DWORD		GetBytesRead(void);
	DWORD		GetBytesPlayed(void);
	BOOL		Stop(void);
	BOOL		Pause(void);
	BOOL		Resume(void);
	BOOL		SetVolume(WORD vol, WORD factor);
	BOOL		SetPan(WORD panPercent, WORD factor);
	void		SetLoop(BOOL loop);
	BOOL		Service(LPSTR sampPtr);
	LPSTR		GetErrorMsg(void);
	void		DumpBuffer(int dumpCount, int n);

	protected:
	void	PrimeBuffer(LPSTR sampPtr);
	BOOL	FillBuffer(LPSTR sampPtr, int startPos, int writeLen);
	BOOL	CopyData(LPSTR sampPtr, LPSTR writePtr, DWORD writeLen, BOOL FirstCopy);
	LONG	LogVolume(WORD volume, WORD factor);

	DWORD						secondaryBuffSize;
	#if 0	// GWP Bad idea
	LPVOID					lpDSoundBuf;
	#endif
	DWORD						bytesPerSec;
	DWORD						readPos;
	DWORD						waveSize;
	DWORD						bytesLeft;
	DWORD						totBytesRead;
	DWORD						prevDriverBytesPlayed;
	DWORD						prevBytesPlayed;
	DWORD						lastBytesPlayed;
	DWORD						nextWritePosition;
	DSBUFFERDESC			dsbd;
	WAVEFORMATEX			wfx;
	LPDIRECTSOUNDBUFFER	pDSB;
	BOOL						loopSnd;
	BOOL						nowPlaying;
	BOOL						nowPaused;
	BOOL						bufferPrimed;
	DWORD						finishTime;
	DWORD						prevTime;
	DWORD						prevDriverTime;
	DWORD						pauseTime;
	char						mBuff[50];
	int						zeroCount;
	BOOL						Initialized;
	BOOL						finishedPlaying;
	BOOL						inService;
	BOOL						buffStatic;
	BOOL						bpFirst;
	WORD						howPlay;
	int						silence;
};

#endif
