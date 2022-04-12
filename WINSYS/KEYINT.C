/* ========================================================================
   Copyright (c) 1990,1995   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: KEYINT.C    -Handles keyboard input and installing the interrupt
   Author: Chris Phillips
   ========================================================================
   Contains the following internal functions: 
   key_rtn               -used as our new keyboard interrupt

   Contains the following general functions:
   install_keyint        -installs the new interrupt
   remove_keyint         -reinstalls the old interrupt, removes the new one
   wait_for_space        -wait for a the spacebar to be pressed
   keys_down             -checks to see if any keys are down
   key_status            -returns the status of a key
   
   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <string.h>

#include "../SYSTEM.H"
#include "../MACHINE.H"
#include "../MACHINT.H"
#include "../ENGINE.H"

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
volatile unsigned char key_stat[256];
volatile BOOL fAnyKeyChanged;


/* =======================================================================
   Function    - install_keyint 
   Description - installs the keyboard interrupt
   Returns     - void
   ======================================================================== */
void install_keyint()
{
}

/* =======================================================================
   Function    - remove_keyint
   Description - removes the installed keyboard interrupt
   Returns     - void
   ======================================================================== */
void remove_keyint()
{
}

/* =======================================================================
   Function    - keys_down
   Description - scans the key_stat array to see if any keys are being pressed
   Returns     - TRUE if there are any keys down, false otherwise
   ======================================================================== */
long keys_down()
{
        return(FALSE);
}

/* =======================================================================
   Function    - key_status
   Description - returns the status of a specified key
   Returns     - the status of the specified key
   ======================================================================== */
long key_status(long k)
{
        long    rv;

        rv = key_stat[k] & 0x3F;                /* get key status */
        key_stat[k] &= 0xC0;                            /* clear key down */

        return rv;
}


long async_key_status(long k)
{
//      return(GetAsyncKeyState(k) & 0x8000);
        return(key_stat[k] & 0x40);
}

#pragma off (unreferenced)
void clear_key_status(LONG unused)
{
#ifdef _WINDOWS
	ClearMessageQueue();
#endif
	memset((void *)key_stat,0,256);
}
#pragma on (unreferenced)

/*      ======================================================================== */
