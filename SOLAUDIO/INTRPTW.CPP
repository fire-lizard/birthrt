//	intrptw.cpp
//		Timer callback interrupt handler for Win95 only.

#define INCLUDE_MMSYSTEM_H
#include "windows.h"
#include "mmsystem.h"
#include "intrptw.hpp"
#ifndef AUDIO_LIB
#else
#include "custmsys.hpp"
#endif

static	Boolean		SetTimer(Boolean * /*TimersStillRunning */);
static	Boolean		StartTimer(int freqMSec);
static	void		StopTimer(void);

struct Server {
	Boolean		inUse;		// is server active?
	int			freq;			// desired freqency of service (MSec)
	int			prevTime;	// system time of last service
	ServerFunc*	function;	// pointer to server function
} servers[8];

static	UINT		timerId;
static	UINT		timerPeriod;
static	UINT		timerAccuracy		= 10;
static	UINT		timerMaxPeriod	= 1000;
static	Boolean	timerRunning		= False;
static	Boolean	anyServers;

Boolean
InstallServer(ServerFunc* proc, int freqMSec)
{
	// install a server at the head of the timer interrupt chain.  'proc'
	// is a pointer to the server procedure.
	
	Server*	cur;
	Server*	freeSlot = 0;
	Boolean Success = True;
	TIMECAPS		timerCapability;
	
	for (cur = servers; cur < servers + NEls(servers); cur++)
		// check to see if this server is already in the list.  If so, return
		if (cur->inUse) {
			if (cur->function == proc)
				return Success;

		//	otherwise, save this slot
		} else if (!freeSlot)
			freeSlot = cur;
	
	if (!freeSlot)
		#ifndef AUDIO_LIB
		//msgMgr->Fatal(SrcLoc, Msg_MaxServers);
		#else
		# if defined(_DEBUG)
		msgMgr->Fatal("Maximum number of servers exceeded");
		#else
		Success = False;
		#endif
		#endif

	if (Success == True)
	{
		// Get this machine's capablity.
		if (timeGetDevCaps(&timerCapability, sizeof (timerCapability)) != TIMERR_NOERROR)
		{
			Success = False;
		}
		else
		{
			Boolean TimersStillRunning = False;
			
			timerAccuracy = max(10, timerCapability.wPeriodMin);
			timerMaxPeriod = timerCapability.wPeriodMax;
			// fire up the timer if not already running
			Success = SetTimer(&TimersStillRunning);
			if (Success == True)
			{
				freeSlot->function	= proc;
				freeSlot->inUse		= True;
				freeSlot->freq		= freqMSec - timerAccuracy;	// incase we are called a bit too soon.
				freeSlot->prevTime	= 0;
			}
		}
	}

	return Success;
}

Boolean
DisposeServer(ServerFunc* proc)
{
	Boolean TimersStillRunning = False;
	// remove the timer interrupt server pointed to by 'proc' from 
	//	the server chain.

Server*	cur;
	
	for (cur = servers; cur < servers + NEls(servers); cur++)
		if (cur->inUse && cur->function == proc) {
			cur->inUse = False;
			break;
		}

	// stop the timer if no servers left
	SetTimer(&TimersStillRunning);
	return TimersStillRunning;
}

// Return whether we were successful in the requested operation of either
// starting up a timer or shutting down the used ones.
static Boolean
SetTimer(Boolean *TimersStillRunning)
{
int	lowFreq;
Server*	cur;
Boolean Success = True;

	// determine if any servers are installed, and if so, the lowest frequency
	anyServers = False;
	lowFreq = 10000;
	for (cur = servers; cur < servers + NEls(servers); cur++) {
		if (cur->inUse) {
			anyServers = True;
			if ((cur->freq) < lowFreq)
				lowFreq = cur->freq;
		}
	}

	// if no servers and timer is running, then kill the timer
	if (!anyServers) {
		if (timerRunning)
			StopTimer();
		*TimersStillRunning = False;
		return Success;
	}

	// one or more servers are running
	// if timer is not running, then fire it up
	if (!timerRunning) {
		*TimersStillRunning = Success = StartTimer(lowFreq);
		return Success;
	}

	// if timer is running, then see if we need to change the frequency
	if (lowFreq != (int)timerPeriod) {
		// stop the timer and then start it with the new frequency
		StopTimer();
		*TimersStillRunning = Success = StartTimer(lowFreq);
	}
	
	return Success;
}

static Boolean
StartTimer(int freqMSec)
{
	timerPeriod = (UINT)freqMSec;
	// Make sure we are within acceptable bounds.
	if (timerPeriod < timerAccuracy)
	{
		timerPeriod = timerAccuracy;
	}
	else if (timerPeriod > timerMaxPeriod)
	{
		timerPeriod = timerMaxPeriod;
	}
	timeBeginPeriod (timerPeriod);
	timerId = timeSetEvent (timerPeriod, timerAccuracy, (LPTIMECALLBACK)TimeFunc,
								(DWORD)0, TIME_PERIODIC);
	timerRunning = (timerId != NULL) ? True : False;
	return timerRunning;
}

static void
StopTimer(void)
{
	if (timerRunning == True)
	{
		timeKillEvent(timerId);
		timeEndPeriod(timerPeriod);
		timerRunning = False;
	}
}

void CALLBACK
TimeFunc(UINT uTimerID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2)
{
static Boolean ServiceLock;
Server*	cur;
int		currTime, elapsedTime;

	if (ServiceLock == True)
		return;
	ServiceLock = True;
	// get current system time
	currTime		= (int)timeGetTime();

	//Call each server in the list.
	for (cur = servers; cur < servers + NEls(servers); cur++) {
		if (cur->inUse) {
			elapsedTime = currTime - cur->prevTime;
			// call server if enough time has elapsed
			if (elapsedTime >= cur->freq) {
				cur->prevTime = currTime;
				(*(cur->function))();
			}
		}
	}
	
	ServiceLock = False;
}
