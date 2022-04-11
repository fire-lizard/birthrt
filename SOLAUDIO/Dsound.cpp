// dsound.cpp

#include "windows.h"
#include <dsound.h>
#include "dsound.hpp"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <io.h>

/**********************************
To Do:

	- Put sample directly in secondary buffer if it will fit
	- Investigate using DuplicateSoundBuffer
	- What about GetBytesPlayed, etc for paused samples?
*/

#define	DSOUND_NOLIB			8078
#define	DSOUND_NOPROC			8079
#define	DSOUND_SIZE_ERROR		8080
#define	DSOUND_NO_RESPONSE	8081

typedef HRESULT 	 (FAR PASCAL * FPDIRECTSOUNDCREATE)
							(GUID FAR *, LPDIRECTSOUND *, IUnknown FAR *);

static	void	DSDisplayError(HRESULT hr, LPSTR title, BOOL inService);
static	void	FixWaveFormat(WAVEFORMATEX *wfx);

static	FPDIRECTSOUNDCREATE	fpDirectSoundCreate;
static	HINSTANCE				hLib;
static	HRESULT					hr;
static	LPDIRECTSOUND			gpds = NULL;
static	DSBUFFERDESC			primaryDsbd;
static	LPDIRECTSOUNDBUFFER	primaryDSB;
static	char						errBuff[255];


BOOL
DetectDSound(void)

//	Determine if Direct Sound is installed and useable on the system.
//	Returns TRUE if available, FALSE otherwise.

{
	// try to load the direct sound library
	hLib = LoadLibrary("dsound.dll");
	if (hLib == NULL) {
		return FALSE;
	}

	// get the library entry point for create function
	fpDirectSoundCreate = (FPDIRECTSOUNDCREATE)GetProcAddress(hLib, "DirectSoundCreate");
	if (fpDirectSoundCreate == NULL) {
		FreeLibrary(hLib);
		return FALSE;
	}

	// see if we can do a create
	hr = (*fpDirectSoundCreate)( NULL, &gpds, NULL );
	if(hr != DS_OK) {
		FreeLibrary(hLib);
		return FALSE;
	}

	// everything OK - release, free the library, and return
	gpds->Release();
	FreeLibrary(hLib);
	return TRUE;
}

BOOL
InitDSound(HWND hMyWnd)

//	Initialize the Direct Sound system.  Must be called prior to playing any
//	samples.

{
	// load the direct sound library
	hLib = LoadLibrary("dsound.dll");
	if (hLib == NULL) {
		DSDisplayError(DSOUND_NOLIB,"LoadLibrary",FALSE);
		return FALSE;
	}

	// get the library entry point for create function
	fpDirectSoundCreate = (FPDIRECTSOUNDCREATE)GetProcAddress(hLib, "DirectSoundCreate");
	if (fpDirectSoundCreate == NULL) {
		DSDisplayError(DSOUND_NOPROC,"GetProcAddress",FALSE);
		FreeLibrary(hLib);
		return FALSE;
	}

	// create a direct sound object
	hr = (*fpDirectSoundCreate)( NULL, &gpds, NULL );
	if(hr != DS_OK) {
		DSDisplayError(hr,"DirectSoundCreate1",FALSE);
		FreeLibrary(hLib);
		return FALSE;
	}

	gpds->Release();

	// done a second time to clear previous use which was not cleaned up
	// (this is a bit of magic, but seems to work)
	hr = (*fpDirectSoundCreate)( NULL, &gpds, NULL );
	if(hr != DS_OK) {
		DSDisplayError(hr,"DirectSoundCreate2",FALSE);
		FreeLibrary(hLib);
		return FALSE;
	}

	// set cooperative level to PRIORITY so we can change the format
	hr = gpds->SetCooperativeLevel(hMyWnd, DSSCL_PRIORITY);
	if( hr != DS_OK ) {
		DSDisplayError(hr,"SetCooperativeLevel",FALSE);
		gpds->Release();
		FreeLibrary(hLib);
		return FALSE;
	}

	// Set up the primary buffer so we can determine it's format
	memset(&primaryDsbd, 0, sizeof(DSBUFFERDESC));
	primaryDsbd.dwSize                 = sizeof(DSBUFFERDESC);
	primaryDsbd.dwFlags                = 0;
	primaryDsbd.dwFlags                |= DSBCAPS_PRIMARYBUFFER;
	primaryDsbd.dwBufferBytes          = 0;
	primaryDsbd.lpwfxFormat            = NULL;

	hr = gpds->CreateSoundBuffer((LPDSBUFFERDESC)&primaryDsbd,
											(LPDIRECTSOUNDBUFFER *)&primaryDSB,
											NULL);
	if( hr != DS_OK ) {
		DSDisplayError(hr,"CreateSoundBuffer primary",FALSE);
		return FALSE;
	}

	return TRUE;
}

BOOL
TermDSound(void)

//	Terminate access to direct sound.  Should be called prior to program
//	termination.

{
	gpds->Release();
	FreeLibrary(hLib);
	return TRUE;
}

BOOL
GetCaps(LPDSCAPS lpDsCaps)

//	Get the direct sound device capabilities into a DSCAPS structure.

{
	memset(lpDsCaps, 0, sizeof(DSCAPS));
	lpDsCaps->dwSize = sizeof(DSCAPS);

	hr = gpds->GetCaps(lpDsCaps);
	if( hr != DS_OK ) {
		DSDisplayError(hr,"GetCaps",FALSE);
		return FALSE;
	}
	return TRUE;
}

BOOL
GetPrimaryFormat(DWORD *rate, WORD *bits, WORD *channels)

//	Returns the current format of the primary direct sound playback buffer.

{
WAVEFORMATEX	getWfx;

	// get the format into a WAVEFORMATEX structure
	hr = primaryDSB->GetFormat(&getWfx,sizeof(WAVEFORMATEX),NULL);
	if( hr != DS_OK ) {
		DSDisplayError(hr,"GetFormat",FALSE);
		return FALSE;
	}

	*rate			= getWfx.nSamplesPerSec;
	*bits			= getWfx.wBitsPerSample;
	*channels	= getWfx.nChannels;

	return TRUE;
}

BOOL
SetPrimaryFormat(DWORD *rate, WORD *bits, WORD *channels)

//	Sets the format of the primary direct sound playback buffer.  The format
//	must be one that the device is capable of playing (see GetCaps).

{
WAVEFORMATEX	setWfx;

	setWfx.wFormatTag			= WAVE_FORMAT_PCM;
	setWfx.nSamplesPerSec	= *rate;
	setWfx.wBitsPerSample	= *bits;
	setWfx.nChannels 			= *channels;
	FixWaveFormat(&setWfx);

	hr = primaryDSB->SetFormat(&setWfx);
	if( hr != DS_OK ) {
		DSDisplayError(hr,"SetFormat",FALSE);
		return FALSE;
	}

	return TRUE;
}

DSound::DSound()

//	DSound object constructor

{
	readPos			= 0;
	totBytesRead	= 0;
	Initialized		= FALSE;
	nowPlaying		= FALSE;
	nowPaused		= FALSE;
	bufferPrimed	= FALSE;
	loopSnd			= FALSE;
	inService		= FALSE;
	nextWritePosition = 0;	// Index to the begining of our secondary buffer.
}

DSound::~DSound()

//	DSound object destructor

{
	if (Initialized) {
		Stop();
		pDSB->Release();
	}
}

BOOL
DSound::Init(DWORD buffSize,LPSTR sampPtr,DWORD sampSize,WORD bits,DWORD rate,WORD channels)

//	Initialize the DSound object prior to playing it.

{
LPVOID		buffPtr1, buffPtr2;
DWORD			buffLen1, buffLen2;

	// if secondary buffer size is zero, then this a static buffer which
	// will be loaded and played in its entirety
	if (buffSize == 0) {
		buffStatic	= TRUE;
		secondaryBuffSize	= sampSize;
	} else {
		buffStatic	= FALSE;
		secondaryBuffSize	= buffSize;
	}

	waveSize			= sampSize;
	totBytesRead	= 0;
	nowPlaying		= FALSE;
	nowPaused		= FALSE;
	bufferPrimed	= FALSE;
	if (bits == 8)
		silence = 0x80;
	else
		silence = 0x00;

	// set up WAVEFORMATEX
	wfx.nChannels			= channels;
	wfx.nSamplesPerSec	= rate;
	wfx.wBitsPerSample	= bits;
	FixWaveFormat(&wfx);

	// need for position calculations
	bytesPerSec = rate * (bits / 8) * channels;

	// Set up the direct sound buffer. 
	memset(&dsbd, 0, sizeof(DSBUFFERDESC));
	dsbd.dwSize                 = sizeof(DSBUFFERDESC);
	dsbd.dwFlags                = 0;
	if (buffStatic)
		dsbd.dwFlags             |= DSBCAPS_STATIC;
	dsbd.dwFlags                |= DSBCAPS_CTRLDEFAULT;
	dsbd.dwFlags                |= DSBCAPS_LOCSOFTWARE;
//	dsbd.dwFlags                |= DSBCAPS_GLOBALFOCUS;
	dsbd.dwBufferBytes          = secondaryBuffSize;
	dsbd.lpwfxFormat            = (LPWAVEFORMATEX)&wfx;

	hr = gpds->CreateSoundBuffer((LPDSBUFFERDESC)&dsbd,
											(LPDIRECTSOUNDBUFFER *)&pDSB,
											NULL);
	if( hr != DS_OK ) {
		DSDisplayError(hr,"CreateSoundBuffer",FALSE);
		return FALSE;
	}

	#if 0
	// GWP This is really bad. (The address will move after the Unlock call.
	
	// get the starting address of the buffer for later reference
	hr = pDSB->Lock(0,1,&buffPtr1,&buffLen1,&buffPtr2,&buffLen2,0);
	if( hr != DS_OK ) {
		DSDisplayError(hr,"Lock",FALSE);
		return FALSE;
	}
	lpDSoundBuf = buffPtr1;

	// unlock the buffer
	hr = pDSB->Unlock(buffPtr1,buffLen1,buffPtr2,buffLen2);
	if( hr != DS_OK ) {
		DSDisplayError(hr,"Unlock",FALSE);
		return FALSE;
	}
	#endif

	// fill the buffer so it's ready to play
	PrimeBuffer(sampPtr);

	Initialized = TRUE;

	return TRUE;
}

BOOL
DSound::Play(LPSTR sampPtr)

//	Start playing the sample.  Play may be called repeatedly to restart the
//	sample.

{
	if (!Initialized)
		return FALSE;

	if (nowPlaying)
		Stop();

	// fill the buffer again in case this is a restart (won't do it twice)
	PrimeBuffer(sampPtr);

	// start the play
	if (!buffStatic || loopSnd)
		howPlay = DSBPLAY_LOOPING;
	else
		howPlay = 0;

	hr = pDSB->Play(0,0,howPlay);
	if( hr != DS_OK ) {
		DSDisplayError(hr,"Play",FALSE);
		return FALSE;
	}

	// initialize values for GetBytesPlayed function
	prevTime			= timeGetTime();
	prevDriverTime	= prevTime;
	pauseTime		= prevTime;
	prevDriverBytesPlayed	= 0;
	prevBytesPlayed			= 0;
	lastBytesPlayed			= 0;

	// If static, non-looping buffer, then ompute the finish time
	if (buffStatic && !loopSnd) {
		finishTime = prevTime +
				((waveSize * 8000) / (wfx.wBitsPerSample * wfx.nSamplesPerSec * wfx.nChannels))
				+ 20;
	}

	nowPlaying		= TRUE;
	nowPaused		= FALSE;
	bufferPrimed	= FALSE;
	zeroCount		= 0;

	return TRUE;
}

BOOL
DSound::IsPlaying(void)
{
	if (buffStatic && !loopSnd)
		if (timeGetTime() > finishTime)
			nowPlaying = FALSE;

	return (nowPlaying || nowPaused);
}

DWORD
DSound::GetReadPos(void)

//	Return the sample's byte index position of the byte which will be read
//	next into the DSound buffer.  I.e. bytes prior to this have already been
//	processed and may be overwritten.

{
	return readPos;
}

DWORD
DSound::GetBytesRead(void)

//	Return the total number of bytes which have been read so far.  This is
//	useful for tracking looping buffers which must be back-filled.

{
	return totBytesRead;
}

DWORD
DSound::GetBytesPlayed(void)

//	Return the total number of bytes which have been played so far.  This is
//	always less than or equal to GetBytesRead and represents the current
//	play position.

{
DWORD	dsPlayPos, writePos, nextPos;
DWORD	writeLen;
DWORD	driverBytesPlayed;
DWORD	bytesPlayed;
DWORD	currTime;
DWORD	elapsedTime;

	if (!nowPlaying)
		return 0;

	// get the current play and write positions
	hr = pDSB->GetCurrentPosition(&dsPlayPos,&writePos);
	if( hr != DS_OK )
		DSDisplayError(hr,"GetCurrentPosition",FALSE);
	nextPos = nextWritePosition;

	// examine the arrangement of play, write, and next positions to
	// determine what the next write length would be - i.e. how much of the
	// secondary buffer can be filled.  
	// dsPlayPos = position which is currently playing
	// writePos  = position at which it is safe to write
	// nextPos   = next position into which we will write (one more than last write)

	if (dsPlayPos < writePos) {
		if (dsPlayPos < nextPos) {
			if (writePos < nextPos) {
				// play, write, next
				writeLen = secondaryBuffSize - nextPos + dsPlayPos;
			} else {
				// play, next, write (error)
				writeLen = secondaryBuffSize - writePos + dsPlayPos;
			}
		} else {
				// next, play, write
				writeLen = dsPlayPos - nextPos;
		}
	} else {
		if (writePos < nextPos) {
			if (nextPos < dsPlayPos) {
				// write, next, play
				writeLen = dsPlayPos - nextPos;
			} else {
				// write, play, next (error)
				writeLen = dsPlayPos - writePos;
			}
		} else {
				// next, write, play (error)
				writeLen = dsPlayPos - writePos;
		}
	}

	// subtracting the write length from the secondary buffer size leaves
	// the amount which has been read but not yet played.  Subtracting this
	// from totBytesRead (of the input buffer) gives the total bytes which
	// have been played.

	driverBytesPlayed = totBytesRead - (secondaryBuffSize - writeLen);

	// The following code is to compensate for a problem exhibited by some
	// Direct Sound drivers.  That is, instead of returning a coutinously
	// increasing value for play position, these drivers return the same
	// value for a time, and then make a large jump in value.  The result is 
	// a staircase function.  This causes the Duck movie player to be jerky.
	// The solution is to return a value based on elapsed time, with
	// occasional (every three seconds) corrections back to the returned
	// value in order to control rate drift.

	currTime = timeGetTime();

	elapsedTime = currTime - prevTime;

	// make a drift correction every 3 seconds
	if (elapsedTime > 3000) {
		prevTime = prevDriverTime;
		prevBytesPlayed = prevDriverBytesPlayed;
		elapsedTime = currTime - prevTime;
	}

	// compute the bytes played based on elapsed time
	bytesPlayed = prevBytesPlayed + ((elapsedTime * bytesPerSec) / 1000);

	// whenever the returned value changes, save the values prior to the
	// change for when we do a drift correction
	if (driverBytesPlayed != prevDriverBytesPlayed) {
		prevDriverBytesPlayed = driverBytesPlayed;
		prevDriverTime = currTime;
	}

	// make sure we don't go backward
	if (bytesPlayed < lastBytesPlayed)
		bytesPlayed = lastBytesPlayed;

	lastBytesPlayed = bytesPlayed;

	return bytesPlayed;
}

BOOL
DSound::Stop(void)

//	Stop playing the sample.

{
	if (!nowPlaying)
		return TRUE;

	nowPlaying = FALSE;
	hr = pDSB->Stop();
	if( hr != DS_OK ) {
		DSDisplayError(hr,"Stop",FALSE);
		return FALSE;
	}
	return TRUE;
}

BOOL
DSound::Pause(void)

//	Pause the sample.

{
	// just stop it
	nowPaused	= TRUE;
	nowPlaying	= FALSE;
	pauseTime = timeGetTime();
	hr = pDSB->Stop();
	if( hr != DS_OK ) {
		DSDisplayError(hr,"Pause",FALSE);
		return FALSE;
	}
	return TRUE;
}

BOOL
DSound::Resume(void)

//	Resume playing after a pause.

{
	if (!Initialized)
		return FALSE;

	// start playing at the current position
	hr = pDSB->Play(0,0,howPlay);
	if( hr != DS_OK ) {
		DSDisplayError(hr,"Resume",FALSE);
		return FALSE;
	}
	prevDriverTime += timeGetTime() - pauseTime;
	prevTime += timeGetTime() - pauseTime;
	nowPlaying		= TRUE;
	nowPaused		= FALSE;
	return TRUE;
}

BOOL
DSound::SetVolume(WORD vol, WORD factor)

//	Set the sample's volume.  This may be called before or during playback.
//	Volume level from 0 (silence) to 100 (full volume).

{
	if (!Initialized)
		return FALSE;

	hr = pDSB->SetVolume(LogVolume(vol,factor));
	if( hr != DS_OK ) {
		DSDisplayError(hr,"SetVolume",FALSE);
		return FALSE;
	}
	return TRUE;
}

BOOL
DSound::SetPan(WORD panPercent, WORD factor)

// Set the pan value to panPercent which is the percentage
// value for the right channel.  A value of 50 means centered.

// In the directSound call, -10,000 is full left, 0 is centered, and
// 10,000 is full right.

{
LONG	newPan;

	if (!Initialized)
		return FALSE;

	// compute the directSound value
	if (panPercent < 50)
		newPan = LogVolume(panPercent * 2, factor);
	else
		newPan = 0 - LogVolume((100 - panPercent) * 2, factor);

	hr = pDSB->SetPan(newPan);
	if( hr != DS_OK ) {
		DSDisplayError(hr,"SetPan",FALSE);
		return FALSE;
	}
	return TRUE;
}

void
DSound::SetLoop(BOOL loop)

//	Set the loop property of the sample.  This may be changed while the
//	sample is playing.

{
	loopSnd = loop;
}

void
DSound::DumpBuffer(int dumpCount, int n)

// Dump the contents of the secondary buffer to disk file disk file
// "dsound.<dumpCount><n>".  Used for debugging.

{
LPVOID		buffPtr1, buffPtr2;
DWORD			buffLen1, buffLen2;
int			fd;
char			mbuff[30];

	Pause();

	hr = pDSB->Lock(0,1,&buffPtr1,&buffLen1,&buffPtr2,&buffLen2,0);
	if( hr != DS_OK ) {
		DSDisplayError(hr,"Lock",FALSE);
		return;
	}

	sprintf(mbuff,"dsound.%d%d",dumpCount,n);
	fd = open(mbuff, O_CREAT | O_TRUNC | O_BINARY | O_RDWR, S_IWRITE);
	write(fd,buffPtr1,secondaryBuffSize);
	close(fd);

	Resume();
}

BOOL
DSound::Service(LPSTR sampPtr)

//	Service the playback process.  This must be called frequently enough
//	to maintain continuous playback.  The sample should not be allowed to move
//	while in this routine.

{
DWORD	dsPlayPos, writePos, nextPos;
DWORD	startPos, writeLen;
DWORD	bytesToPlay;

	if (!nowPlaying)
		return TRUE;

	if (buffStatic)
		return TRUE;

	// stop if the sample is finished
	if ((bytesLeft == 0) &&
		(timeGetTime() > finishTime)) {
		Stop();
		return TRUE;
	}

	inService = TRUE;

	// get the current play and write positions
	hr = pDSB->GetCurrentPosition(&dsPlayPos,&writePos);
	if( hr != DS_OK ) {
		DSDisplayError(hr,"GetCurrentPosition",inService);
		inService = FALSE;
		return FALSE;
	}
	nextPos = nextWritePosition;


	// examine the arrangement of play, write, and next positions to
	// determine what the next write length should be - i.e. how much of the
	// secondary buffer can be filled and where it starts.  
	// dsPlayPos = position which is currently playing
	// writePos  = position at which it is safe to write
	// nextPos   = next position into which we will write (one more than last write)

	if (dsPlayPos < writePos) {
		if (dsPlayPos < nextPos) {
			if (writePos < nextPos) {
				// play, write, next
				startPos = nextPos;
				writeLen = secondaryBuffSize - startPos + dsPlayPos;
			} else {
				// play, next, write (error)
				startPos = writePos;
				writeLen = secondaryBuffSize - startPos + dsPlayPos;
//				OutputDebugString("err 1\n");
			}
		} else {
				// next, play, write
				startPos = nextPos;
				writeLen = dsPlayPos - startPos;
		}
	} else {
		if (writePos < nextPos) {
			if (nextPos < dsPlayPos) {
				// write, next, play
				startPos = nextPos;
				writeLen = dsPlayPos - startPos;
			} else {
				// write, play, next (error)
				startPos = writePos;
				writeLen = dsPlayPos - startPos;
//				OutputDebugString("err 2\n");
			}
		} else {
				// next, write, play (error)
				startPos = writePos;
				writeLen = dsPlayPos - startPos;
//				OutputDebugString("err 3\n");
		}
	}

	// if the write length is zero, nothing to do
	// (if this happens a lot, it means that something got stuck)
	if (!writeLen) {
#ifdef _DEBUG
		zeroCount++;
		if (zeroCount > 10) {
			DSDisplayError(DSOUND_NO_RESPONSE,"No Response",inService);
			inService = FALSE;
			return FALSE;
		}
#endif
		return TRUE;
	}
	zeroCount = 0;

	// if this will exhaust the buffer, then compute the finish time
	if (!loopSnd &&
			(bytesLeft > 0) &&
			(bytesLeft <= writeLen)) {
		bytesToPlay = secondaryBuffSize - (writeLen - bytesLeft);
		finishTime = timeGetTime() +
				((bytesToPlay * 8000) / (wfx.wBitsPerSample * wfx.nSamplesPerSec * wfx.nChannels))
				+ 20;
	}

	// now, fill the buffer at the calculated start position for the
	// calculated length

  	return FillBuffer(sampPtr,startPos,writeLen);
}

LPSTR
DSound::GetErrorMsg(void)

// Return pointer to the error message text buffer.  This should be called
// if the service routine returns FALSE indicating an error occured.

{
	return (LPSTR)errBuff;
}

void
DSound::PrimeBuffer(LPSTR sampPtr)

// pre-fill the buffer if it has not already been filled

{
	if (!bufferPrimed) {
		if (!nowPlaying) {
			hr = pDSB->SetCurrentPosition(0);
			if( hr != DS_OK )
				DSDisplayError(hr,"SetCurrentPosition",inService);
		}
		bytesLeft = waveSize;
		readPos = 0;
		if (!nowPlaying)
			FillBuffer(sampPtr,0,secondaryBuffSize);
		bufferPrimed = TRUE;
	}
}


BOOL
DSound::FillBuffer(LPSTR sampPtr, int startPos, int writeLen)

// fill the secondary buffer starting at startPos for writeLen bytes

{
LPVOID		buffPtr1, buffPtr2;
DWORD			buffLen1, buffLen2;
BOOL			result;

	// lock the buffer and get the current writable positions (2 in case it wraps)
	hr = pDSB->Lock(startPos,writeLen,&buffPtr1,&buffLen1,&buffPtr2,&buffLen2,0);

	// If we got DSERR_BUFFERLOST, restore and retry lock
	if (hr == DSERR_BUFFERLOST) {
		hr = pDSB->Restore();
		hr = pDSB->Lock(startPos,writeLen,&buffPtr1,&buffLen1,&buffPtr2,&buffLen2,0);
	}

	if( hr != DS_OK ) {
		DSDisplayError(hr,"Fill Lock",inService);
		return FALSE;
	}

	// copy the first piece of data
	result = CopyData(sampPtr,(LPSTR)buffPtr1,buffLen1, TRUE);
	if (!result)
		return FALSE;

	// if it wrapped, copy the second piece
	if (buffPtr2 != NULL) {
		#if 0 // GWP Bad idea.
		// save starting address
		lpDSoundBuf = buffPtr2;
		#endif
		result = CopyData(sampPtr,(LPSTR)buffPtr2,buffLen2, FALSE);
		if (!result)
			return FALSE;
	}

	// now unlock the buffer
	hr = pDSB->Unlock(buffPtr1,buffLen1,buffPtr2,buffLen2);
	if( hr != DS_OK ) {
		DSDisplayError(hr,"Fill Unlock",inService);
		return FALSE;
	}
	return TRUE;
}

BOOL
DSound::CopyData(LPSTR sampPtr, LPSTR writePtr, DWORD writeLen, BOOL FirstBuffer)

// copy data from the input buffer to the secondary buffer starting at
// writePtr for writeLen bytes

{
LPSTR		newWritePtr;
DWORD		newWriteLen;
DWORD	 	nextPos;

	if (bytesLeft > writeLen) {
		// normal case - write full length to buffer
		memcpy(writePtr,sampPtr + readPos,writeLen);
		bytesLeft -= writeLen;
		totBytesRead += writeLen;
		readPos += writeLen;
	} else {
		// partial write - first write what we have left
		if (bytesLeft > 0)
			memcpy(writePtr,sampPtr + readPos,bytesLeft);
		newWriteLen = writeLen - bytesLeft;
		newWritePtr = writePtr + bytesLeft;
		totBytesRead += bytesLeft;
		if (loopSnd) {
			// if looping, write rest from beginning
			memcpy(newWritePtr,sampPtr,newWriteLen);
			bytesLeft = waveSize - newWriteLen;
			totBytesRead += newWriteLen;
			readPos = newWriteLen;
		} else {
			// not looping, write rest as silence
			memset(newWritePtr,silence,newWriteLen);
			readPos += bytesLeft;
			bytesLeft = 0;
		}
	}

	// compute the next position in the buffer at which we will write
#ifdef _DEBUG
//	int size = (int)(writePtr - (LPSTR)lpDSoundBuf);
//	if ((size > (int)secondaryBuffSize) || (size < 0)) {
//		hr = DSOUND_SIZE_ERROR;
//		DSDisplayError(hr,"CopyData",inService);
//		return FALSE;
//	}
#endif
	#if 0
	// GWP This uses pointer math, ick.
	nextPos = (DWORD)(writePtr - (LPSTR)lpDSoundBuf) + writeLen;
	if (nextPos >= secondaryBuffSize)
		nextPos = 0;
	nextWritePosition = nextPos;
	#endif
	
	// GWP I think this replaces the block above.
	// nextWritePosition is the index into the secondary buffer where we
	// want to start for the next FillBuffer call.
	// We don't have to worry about the wrap because DSound will give us wrapped
	// buffer pointers.
	nextWritePosition += writeLen;
	if (nextWritePosition >= secondaryBuffSize)
		nextWritePosition = 0;
	
	return TRUE;
}

LONG
DSound::LogVolume(WORD volume, WORD factor)

// scale the volume of 0-100 to a logarithmic value from -10,000 to 0.

{
LONG	newVol;

	volume = min(100,volume);	

	if (volume == 0)
		newVol = -10000;
	else
		newVol = (LONG)((double)2000/(double)factor * log((double)volume/(double)100));

	return newVol;
}

static void
FixWaveFormat(WAVEFORMATEX *wfx)

// compute dependant values in the WAVEFORMATEX structure from the values
// for rate, bits, and channels

{
WORD	nBytesPerSample;

	nBytesPerSample		= wfx->wBitsPerSample / 8;
	wfx->nAvgBytesPerSec	= wfx->nSamplesPerSec * nBytesPerSample * wfx->nChannels;
	wfx->nBlockAlign		= 1 * nBytesPerSample * wfx->nChannels;
	wfx->cbSize				= 0;
	wfx->wFormatTag		= WAVE_FORMAT_PCM;
}

static void
DSDisplayError(HRESULT hr, LPSTR title, BOOL inService)

// Analyse the passed result code and output the appropriate message along
// with "title".  If "inService" is TRUE, don't output the message since
// we may be inside a timer callback thread.  In this case, a call to
// GetErrorMsg will get the error text.

{
#ifdef _DEBUG

	switch (hr) {

		case DSERR_ALLOCATED:
			strcpy(errBuff,"The call failed because resources (such as a priority level)"
								" were already being used by another caller.");
			break;

		case DSERR_CONTROLUNAVAIL:
			strcpy(errBuff,"The control (vol,pan,etc.) requested by the caller is not available.");
			break;

		case DSERR_INVALIDPARAM:
			strcpy(errBuff,"An invalid parameter was passed to the returning function");
			break;

		case DSERR_INVALIDCALL:
			strcpy(errBuff,"This call is not valid for the current state of this object");
			break;

		case DSERR_GENERIC:
			strcpy(errBuff,"An undetermined error occured inside the DSound subsystem");
			break;

		case DSERR_PRIOLEVELNEEDED:
			strcpy(errBuff,"The caller does not have the priority level required for the function to"
								"succeed.");
			break;

		case DSERR_OUTOFMEMORY:
			strcpy(errBuff,"The DSound subsystem couldn't allocate sufficient memory to complete the"
								"caller's request.");
			break;

		case DSERR_BADFORMAT:
			strcpy(errBuff,"The specified WAVE format is not supported");
			break;

		case DSERR_UNSUPPORTED:
			strcpy(errBuff,"The function called is not supported at this time");
			break;

		case DSERR_NODRIVER:
			strcpy(errBuff,"No sound driver is available for use");
			break;

		case DSERR_ALREADYINITIALIZED:
			strcpy(errBuff,"This object is already initialized");
			break;

		case DSERR_NOAGGREGATION:
			strcpy(errBuff,"This object does not support aggregation");
			break;

		case DSERR_BUFFERLOST:
			strcpy(errBuff,"The buffer memory has been lost, and must be Restored.");
			break;

		case DSERR_OTHERAPPHASPRIO:
			strcpy(errBuff,"Another app has a higher priority level, preventing this call from"
								"succeeding.");
			break;

		case DSERR_UNINITIALIZED:
			strcpy(errBuff,"The Initialize() member on the Direct Sound Object has not been"
								"called or called successfully before calls to other members.");
			break;

		case DSOUND_NOLIB:
			strcpy(errBuff,"Library dsound.dll not found.");
			break;

		case DSOUND_NOPROC:
			strcpy(errBuff,"No proc address for DirectSoundCreate.");
			break;

		case DSOUND_SIZE_ERROR:
			strcpy(errBuff,"Secondary buffer size error.");
			break;

		case DSOUND_NO_RESPONSE:
			strcpy(errBuff,"No response from Direct Sound.");
			break;

		default:
			strcpy(errBuff,"Unknown error code");
			break;
	}

	if (!inService)
		MessageBox(NULL,errBuff,title,MB_OK);

#endif	//_DEBUG
}

