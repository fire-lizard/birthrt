/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: LINKLIST.C - Generic link list routines
   Author: 	 Greg Hightower
   ========================================================================
   Contains the following general functions:
   
   LListInit	- Initialize a new link list
   LListAdd		- Add a new node to a link list
   LListDelete	- Delete a node from a link list
   LListInsert	- Insert a existing node to a link list
   LListRemove	- Remove a node from a link list
   
   Contains the following internal functions:
   
   ======================================================================== */
/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "system.h"
#include "linklist.h"

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

/* ========================================================================
   Function    - LListInit
   Description - Initialize a new link list
   Returns     - handle of new node, HDL_NULL on failure
   ======================================================================== */
HDL LListInit ( void )
{
	HDL	NewNode;
	
	/* allocate a new node */
	if( NewNode = NewBlock(sizeof(PLISTNODE)) != HDL_NULL )
	{
		/* fill the new node */
		//plnCurrent = NODE_PTR(NewNode);
		//plnCurrent->Data = HDL_NULL;
		//plnCurrent->Prev = HDL_NULL;
		//plnCurrent->Next = HDL_NULL;
	}
	
	return(NewNode);
}

/* ========================================================================
   Function    - LListAdd
   Description - Add a new node to a link list, if AfterThis is HDL_NULL,
                 then it is added to end of the list
   Returns     - handle of new node, HDL_NULL on failure
   ======================================================================== */
HDL LListAdd (
	HDL	First,
	HDL	Data,
	HDL AfterThis
)
{
	PLISTNODE	plnCurrent;
	HDL			NewNode;
	HDL			CurrNode;
	
	/* must be a valid list head */
	if( First == HDL_NULL )
		return( HDL_NULL );
		
	/* scan forward in the list to the last item */
	/*for (CurrNode = First;
		 CurrNode != AfterThis && CurNode != HDL_NULL;
		 CurrNode = NODE_PTR(CurrNode)->Next) )
	{}*/
	
	/* allocate a new node */
	if( NewNode = NewBlock(sizeof(PLISTNODE)) != HDL_NULL )
	{
		/* fill the new node */
		//plnCurrent = NODE_PTR(NewNode);
		plnCurrent->Data = Data;
		plnCurrent->Prev = CurrNode;
		plnCurrent->Next = HDL_NULL;
	}
	
	return(NewNode);
}

/* ========================================================================
   Function    - LListDelete
   Description - delete a node from a link list
   Returns     - 0 if Delete was not found in the list, else 1
   ======================================================================== */
LONG LListDelete (
	HDL First,
	HDL Delete
)
{
	PLISTNODE	plnCurrent;
	HDL			CurrNode;
	
	/* must be a valid list head */
	if( First == HDL_NULL )
		return( 0 );
		
	/* scan forward in the list to find the item in question */
	/*for (CurrNode = First;
		 CurrNode != Delete;
		 CurrNode = NODE_PTR(CurrNode)->Next )
	{
		if( CurrNode == HDL_NULL)
			return 0;
	}*/
	
	/* delete the node */
	//plnCurrent = NODE_PTR(CurrNode);
	//NODE_PTR(plnCurrent->Prev)->Next = plnCurrent->Next;
	//NODE_PTR(plnCurrent->Next)->Prev = plnCurrent->Prev;
	DisposBlock(CurrNode);
	
	return( 1 );
}

/* ========================================================================
   Function    - LListInsert
   Description - Insert a existing node to a link list, if AfterThis is 
                 HDL_NULL, then it is added to end of the list
   Returns     - always returns 1
   ======================================================================== */
LONG LListInsert (
	HDL	First,
	HDL	Node,
	HDL AfterThis
)
{
	PLISTNODE	plnCurrent;
	HDL			CurrNode;
	
	/* must be a valid list head */
	if( First == HDL_NULL )
		return( 0 );
		
	/* scan forward in the list to the last item */
	/*for (CurrNode = First;
		 CurrNode != AfterThis && CurNode != HDL_NULL;
		 CurrNode = NODE_PTR(CurrNode)->Next )
	{}*/
	
	/* fill the new node */
	//plnCurrent = NODE_PTR(Node);
	//plnCurrent->Prev = CurrNode;
	//plnCurrent->Next = NODE_PTR(CurrNode)->Next;
	
	return( 1 );
}

/* ========================================================================
   Function    - LListRemove
   Description - Remove a node from a link list
   Returns     - handle of removed item, HDL_NULL otherwise
   ======================================================================== */
HDL LListRemove (
	HDL First,
	HDL Remove
)
{
	PLISTNODE	plnCurrent;
	HDL			CurrNode;
	
	/* must be a valid list head */
	if( First == HDL_NULL )
		return( HDL_NULL );
		
	/* scan forward in the list to find the item in question */
	/*for (CurrNode = First;
		 CurrNode != Remove;
		 CurrNode = NODE_PTR(CurrNode)->Next )
	{
		if( CurrNode == HDL_NULL)
			return( HDL_NULL );
	}*/
	
	/* remove the node */
	//plnCurrent = NODE_PTR(CurrNode);
	//NODE_PTR(plnCurrent->Prev)->Next = plnCurrent->Next;
	//NODE_PTR(plnCurrent->Next)->Prev = plnCurrent->Prev;
	
	return( CurrNode );
}

