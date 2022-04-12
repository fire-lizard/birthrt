/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: TIMER.C      -Handles timer timers
   Author: Greg Hightower 
            (from Chris Philips task.c)
   ========================================================================
   Contains the following internal functions: 

   Contains the following general functions:
   init_timer            -initializes the timer list
   close_timer           -closes the timer list (must call this!)
   add_timer             -adds a timer to the list
   remove_timer          -removes a timer from the list
   run_timer             -runs through timer, deciding to execute them or not 

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>

#ifndef _WINDOWS
#include <bios.h>
#endif

#include "SYSTEM.H"
#include "MACHINE.H"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
// the freq values are in 100 of a second

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
typedef struct{
	LONG active;     /*TRUE if valid timer*/
	LONG next;       /*how many ticks till next call -1 == never*/
	LONG freq;       /*how often (ticks) call happens -1 == never again*/
	PFV  func;
	}TIMER;

#define MAX_TIMERS (10)
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
static TIMER timers[MAX_TIMERS];
static const TIMER *pLastTimer = &timers[MAX_TIMERS - 1];

/* =======================================================================
   Function    - init_timers
   Description - initializes (wipes) the timers list
   Returns     - void
   ======================================================================== */
void init_timers()
{
	LONG i;
	for(i=0;i<MAX_TIMERS;i++)
	{
		timers[i].active=FALSE;
	}
}

/* ========================================================================
   Function    - close_timers
   Description - turn of the timer list
   Returns     - void
   ======================================================================== */
void close_timers(void)
{
}

/* =======================================================================
   Function    - add_timer
   Description - adds a timer to the list
   Returns     - void
   ======================================================================== */
/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
   //freq = how often timer will occur -1 = never.

void add_timer(LONG freq,PFV func)
{
	LONG i;

	for(i=0;i<MAX_TIMERS;i++)
	{
		if(timers[i].active==FALSE)
			break;
	}
	if(i==MAX_TIMERS)
	{
		fatal_error("Exceded MAX_TIMERS");
	}

	timers[i].freq=freq;
	if(freq==-1)
		timers[i].next=-1;
	else
		timers[i].next=1;		//happen next frame
	timers[i].func=func;
	timers[i].active=TRUE;
}

/* =======================================================================
   Function    - remove_timer
   Description - removes a timer based on a pointer to a function
   Returns     - void
   ======================================================================== */
void remove_timer(PFV func)
{
	LONG i;

	if(func)
	{
		for(i=0;i<MAX_TIMERS;i++)
		{
			if(timers[i].func==func)
			{
				timers[i].active=FALSE;
			}
		}
	}

}

/* =======================================================================
   Function    - run_timers
   Description - checks all timers for validity, and then maybe executes them
   Returns     - void
   ======================================================================== */
void run_timers()
{
	TIMER *pThisTimer;
	static LONG PreviousTime = 0;
	
	if(PreviousTime == get_time())
		return;
		
	PreviousTime = get_time();
	
	for(pThisTimer = timers;pThisTimer <= pLastTimer; pThisTimer++)
	{
		if(pThisTimer->active==TRUE)
		{
			if(pThisTimer->next!=-1)  /*does it have a time*/
			{
				pThisTimer->next--;
				if(pThisTimer->next==0) /*/ is it time yet?*/
				{
					(*pThisTimer->func)();
					pThisTimer->next=pThisTimer->freq;
				}
			}
		}
	}
}

/* ========================================================================
   Function    - TickDelay
   Description - delay a number of ticks
   Returns     - 
   ======================================================================== */
void TickDelay (LONG NumTicks)
{
	LONG now = get_time();
	while (get_time() < now+NumTicks)
	{
		run_timers();
	}
}
