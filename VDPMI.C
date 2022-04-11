
/*#######################################################################\
#
#	Synergistic Software
#	(c) Copyright 1993-1994  All Rights Reserved.
#	Author:  Greg Hightower
#
\#######################################################################*/

#include <stdio.h>
#include <dos.h>
#include "typedefs.h"

/*#######################################################################\
#
#    Function: D32DosMemAlloc
#
# Description: allocate a block of memory in DOS low memory using the
#              DPMI interface.
#
#     Returns: flat model pointer to the memory
#
\#######################################################################*/
void *
D32DosMemAlloc (
	ULONG size
)
{
#ifndef _WINDOWS
    union REGS r;

    r.x.eax = 0x0100;			/* DPMI allocate DOS memory */
    r.x.ebx = (size + 15) >> 4; /* Number of paragraphs requested */
    int386 (0x31, &r, &r);

    if (r.x.cflag)				/* Failed */
	    return ((ULONG) 0);
    else
	    return (void *) ((r.x.eax & 0xFFFF) << 4);
#else
	return ((ULONG) 0);
#endif
}
