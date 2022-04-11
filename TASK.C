/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: TASK.C      -Handles tasks
   Author: Chris Phillips
   ========================================================================
   Contains the following internal functions: 

   Contains the following general functions:
   init_tasks            -initializes the task list
   add_task              -adds a task to the list
   remove_task           -removes a task from the list
   run_tasks             -runs through tasks, deciding to execute them or not 

   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>

#ifndef _WINDOWS
#include <bios.h>
#endif

#include "task.h"
#include "machine.h"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
	//REVISIT:Do we need any more functionality here?
	//			 Should val be a PTR so it can be modified by the task func.
/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
typedef struct{
	LONG active;     /*TRUE if valid task*/
	LONG next;       /*how many frames till next call -1 == never*/
	LONG freq;       /*how often (frames) call happens -1 == never again*/
	LONG var;        /*Varible (if any) to pass to function*/
	PFVL func;
	}TASK;

#define MAX_TASKS (50)
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
static TASK tasks[MAX_TASKS];
static const TASK *pLastTask = &tasks[MAX_TASKS - 1];

/* =======================================================================
   Function    - init_tasks
   Description - initializes (wipes) the task list
   Returns     - void
   ======================================================================== */
void init_tasks()
{
LONG i;
	for(i=0;i<MAX_TASKS;++i)
		{
		tasks[i].active=FALSE;
		}
}

/* =======================================================================
   Function    - add_task
   Description - adds a task to the list
   Returns     - void
   ======================================================================== */
/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */
   //freq = how often task will occur -1 = never.

void add_task(LONG freq,PFVL func,LONG var)
{
LONG i;

	for(i=0;i<MAX_TASKS;++i)
		{
		if(tasks[i].active==FALSE)
			break;
		}
	if(i==MAX_TASKS)
		{
#if defined(_DEBUG)
		fatal_error("Exceded MAX_TASKS");
#else
		return;
#endif
		}

	tasks[i].freq=freq;
	if(freq==-1)
		tasks[i].next=-1;
	else
		tasks[i].next=1;		//happen next frame
	tasks[i].func=func;
	tasks[i].var=var;
	tasks[i].active=TRUE;
}

/* =======================================================================
   Function    - remove_task
   Description - removes a task based on a pointer to a function
   Returns     - void
   ======================================================================== */
void remove_task(PFVL func)
{
LONG i;

	if(func)
		{
		for(i=0;i<MAX_TASKS;++i)
			{
			if(tasks[i].func==func)
				{
				tasks[i].active=FALSE;
				}
			}
		}

}

/* =======================================================================
   Function    - run_tasks
   Description - checks all tasks for validity, and then maybe executes them
   Returns     - void
   ======================================================================== */
void run_tasks()
{
	TASK *pThisTask;
	
	for(pThisTask = tasks;pThisTask <= pLastTask; ++pThisTask)
		{
		if(pThisTask->active==TRUE)
			{
			if(pThisTask->next!=-1)  /*does it have a time*/
				{
				--pThisTask->next;
				if(pThisTask->next==0) /*/ is it time yet?*/
					{
					(*pThisTask->func)(pThisTask->var);
					pThisTask->next=pThisTask->freq;
					}
				}
			}
		}

}
/* ========================================================================
   Function    - find_task
   Description - Find a task which matches the func
   Returns     - an index to that task or -1
   ======================================================================== */

LONG find_task(PFVL func)
{
	LONG taskIndex;
	TASK *pThisTask;
	
	if (func)
	{
		for(pThisTask = tasks, taskIndex = 0;
		    pThisTask <= pLastTask; 
		    ++pThisTask, ++taskIndex)
		{
			if(pThisTask->active==TRUE
			   && pThisTask->func == func)
			{
				return taskIndex;
			}
		}
	}
	
	return -1;

}

/* ========================================================================
   Function    - pause_task
   Description - Add time back to this task so it won't trigger.
   Returns     - 
   ======================================================================== */

void pause_task(LONG taskIndex)
{
	if (taskIndex >= 0 && taskIndex < MAX_TASKS)
	{
		++tasks[taskIndex].next;
	}
}

