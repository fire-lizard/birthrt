/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: LINKLIST.H - General include stuff for LinkList 
   Author: 	 Greg Hightower
   ======================================================================== */
#ifndef _LINKLIST_H
#define _LINKLIST_H

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */

/* this define covers the type of a memory handle */
#define HDL			long
#define HDL_NULL	-1

typedef	struct _LISTNODE
{
	HDL	Prev;	// handle to prev node
	HDL	Next;	// handle to prev node
	HDL	Data;	// handle to the data
} LISTNODE, *PLISTNODE;

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
#define NODE_PTR(a)		((PNODELIST)BLKPTR((a)))
#define NODE_NEXT(a)	((PNODELIST)BLKPTR((a)))->Next
#define NODE_PREV(a)	((PNODELIST)BLKPTR((a)))->Prev
#define NODE_DATA(a)	((PNODELIST)BLKPTR((a)))->Data

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

extern HDL  LListInit ( void );
extern HDL  LListAdd ( HDL	First, HDL	Data, HDL AfterThis );
extern LONG LListDelete ( HDL First, HDL Delete );
extern LONG LListInsert ( HDL	First, HDL	Node, HDL AfterThis );
extern HDL  LListRemove ( HDL First, HDL Remove );

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
   
#endif // _LINKLIST_H

