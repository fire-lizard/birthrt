/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _HANDLE_HXX
#define _HANDLE_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_MACHINE_H)
#include "machine.h"
#endif

#if !defined(_MEMMANAG_H)
#include "memmanag.h"
#endif


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Class Definition
   ------------------------------------------------------------------------ */
// A template fn to get a data pointer from a handle without locking.
// Note: Its called "Dumb" because it does no reference counting.

template <class T> class DumbHandlePtr
{
public:
	DumbHandlePtr(SHORT const NewHandle)
		: fHandle(NewHandle)
		{
#if defined (MEMORY_CHK)
			if (NewHandle < 0 || abBlockAttr[NewHandle] == UNUSED)
			{
				fatal_error("Handle (%hd) had not been allocated.\n", NewHandle);
			}
			
			if (abBlockAttr[NewHandle] == FREEMEM)
			{
				fatal_error("Handle (%hd) has been free'd.\n", NewHandle);
			}
			
		    // Data size is on 4 byte boundries.
		    // GWP	if (GetDataBlkSize(NewHandle) != sizeof(T))
			   // Some pointers are not fixed size.
			// GWPif (GetDataBlkSize(fHandle) != (sizeof(T) + ((sizeof(T) % sizeof(int)) ? sizeof(int) - (sizeof(T)%sizeof(int)) : 0)))
			// GWP{
		   	// GWP	fatal_error("Mis-Matched Handle (%hd) size (%ld), and sizeof(%ld).\n",
		   	// GWP				NewHandle,GetDataBlkSize(NewHandle), sizeof(T));
			// GWP}
#endif
		}
		
	T *operator->() const
		{	return ((T *)BLKPTR(fHandle));	}
		
	DumbHandlePtr<T> & operator=(SHORT const NewHandle)
		{ 
#if defined (MEMORY_CHK)
			if (NewHandle < 0 || abBlockAttr[NewHandle] == UNUSED)
			{
				fatal_error("Handle (%hd) had not been allocated.\n", NewHandle);
			}
			
			if (abBlockAttr[NewHandle] == FREEMEM)
			{
				fatal_error("Handle (%hd) has been free'd.\n", NewHandle);
			}
			
		    // Data size is on 4 byte boundries.
		    // GWP 	if (GetDataBlkSize(NewHandle) != sizeof(T))
			   // Some pointers are not fixed size.
			// GWP if (GetDataBlkSize(fHandle) != (sizeof(T) + ((sizeof(T) % sizeof(int)) ? sizeof(int) - (sizeof(T)%sizeof(int)) : 0)))
			// GWP {
			// GWP    	fatal_error("Mis-Matched Handle (%hd) size (%ld), and sizeof(%ld).\n",
			// GWP    				NewHandle,GetDataBlkSize(NewHandle), sizeof(T));
		    // GWP }
#endif
			fHandle = NewHandle; 
			return *this;	
		}
		
	DumbHandlePtr<T> & operator=(DumbHandlePtr<T> const &NewDumbHandle)
		{ 
			if (this != &NewDumbHandle)
			{
#if defined (MEMORY_CHK)
				if (NewDumbHandle.fHandle < 0 || abBlockAttr[NewDumbHandle.fHandle] == UNUSED)
				{
					fatal_error("Handle (%hd) had not been allocated.\n", 
															NewDumbHandle.fHandle);
				}
				
				if (abBlockAttr[NewDumbHandle.fHandle] == FREEMEM)
				{
					fatal_error("Handle (%hd) has been free'd.\n", 
														NewDumbHandle.fHandle);
				}
				
			   // Data size is on 4 byte boundries.
			   // GWP 	if (GetDataBlkSize(NewDumbHandle.fHandle) != sizeof(T))
			   // Some pointers are not fixed size.
			   // GWP	if (GetDataBlkSize(fHandle) != (sizeof(T) + ((sizeof(T) % sizeof(int)) ? sizeof(int) - (sizeof(T)%sizeof(int)) : 0)))
			   // GWP	{
			   // GWP		fatal_error("Mis-Matched Handle (%hd) size (%ld), and sizeof(%ld).\n",
			   // GWP					NewDumbHandle.fHandle,
			   // GWP					GetDataBlkSize(NewDumbHandle.fHandle), sizeof(T));
			   // GWP	}
#endif
				fHandle = NewDumbHandle.fHandle;	
			}
			return *this;
		}

	T& operator*() const
		{ return *((T*)BLKPTR(fHandle)); }
	
	SHORT const mfGetHandle() const
		{ return fHandle; }
		
	operator T*() const
		{ return (T*)BLKPTR(fHandle); }

protected:
private:
	SHORT	fHandle;
};

// A template fn to get a data array from a handle without locking.
// Note: Its called "Dumb" because it does no reference counting.

template <class T> class DumbHandleArray
{
public:
	DumbHandleArray(SHORT const NewHandle)
		: fHandle(NewHandle) 
		{ 
#if defined (MEMORY_CHK)
			if (NewHandle < 0 || abBlockAttr[NewHandle] == UNUSED)
			{
				fatal_error("Handle (%hd) had not been allocated.\n", NewHandle);
			}
			
			if (abBlockAttr[NewHandle] == FREEMEM)
			{
				fatal_error("Handle (%hd) has been free'd.\n", NewHandle);
			}
#endif
		}
	
	// index operators
	T & operator[](LONG const index)
	{ 
#if defined (MEMORY_CHK)
		if (index < 0 || index >= (GetDataBlkSize(fHandle)/sizeof(T)))
		{
			fatal_error("ERROR! Index %ld, Outside array bounds %ld from BLKPTR.\n",
								index, (GetDataBlkSize(fHandle)/sizeof(T)));
		}
#endif
		return (((T *) BLKPTR(fHandle))[index]);
 	}
	
	// for const objects.
	T & operator[](LONG const index) const
	{ 
#if defined (MEMORY_CHK)
		if (index < 0 || index >= (GetDataBlkSize(fHandle)/sizeof(T)))
		{
			fatal_error("ERROR! Index %ld, Outside array bounds %ld from BLKPTR.\n",
								index, (GetDataBlkSize(fHandle)/sizeof(T)));
		}
#endif
		return (((T *) BLKPTR(fHandle))[index]);
 	}
	
	DumbHandleArray<T> & operator=(SHORT const NewHandle)
		{ 
#if defined (MEMORY_CHK)
			if (NewHandle < 0 || abBlockAttr[NewHandle] == UNUSED)
			{
				fatal_error("Handle (%hd) had not been allocated.\n", NewHandle);
			}
			
			if (abBlockAttr[NewHandle] == FREEMEM)
			{
				fatal_error("Handle (%hd) has been free'd.\n", NewHandle);
			}
#endif
			fHandle = NewHandle; 
			return *this;	
		}
		
	DumbHandleArray<T> & operator=(DumbHandleArray<T> const &NewDumbHandle)
		{ 
			if (this != &NewDumbHandle)
			{
#if defined (MEMORY_CHK)
				if (NewDumbHandle.fHandle < 0 || abBlockAttr[NewDumbHandle.fHandle] == UNUSED)
				{
					fatal_error("Handle (%hd) had not been allocated.\n", 
											NewDumbHandle.fHandle);
				}
				
				if (abBlockAttr[NewDumbHandle.fHandle] == FREEMEM)
				{
					fatal_error("Handle (%hd) has been free'd.\n", NewDumbHandle.fHandle);
				}
#endif
				fHandle = NewDumbHandle.fHandle;	
			}
			return *this;
		}
		
protected:
private:
	SHORT	fHandle;
};


// No reference counting, relies on going out of scope to clear the lock.
template <class T> class DumbAutoLockPtr
{
public:
	DumbAutoLockPtr(SHORT iHandle)
		  : fHandle(iHandle),
		    fLockFlag(IsLocked(iHandle)),
		    fPtr((T *)BLKPTR(iHandle))
	{	
#if defined (MEMORY_CHK)
		// Some pointers are not a fixed size.
		// GWP if (GetDataBlkSize(fHandle) != (sizeof(T) + ((sizeof(T) % sizeof(int)) ? sizeof(int) - (sizeof(T)%sizeof(int)) : 0)))
		// GWP {
		// GWP 	fatal_error("Handle (%hd) wrong size for type.\n", iHandle);
		// GWP }
#endif
		
		if (!fLockFlag)
		{
			SetQuickLock(iHandle);		// Lock the memory in place
		}
	}
	
	~DumbAutoLockPtr()
	{
		if (!fLockFlag)
		{
			ClrLock(fHandle);
		}
	}
	
	T *operator->() const
		{	return (fPtr);	}
		
	DumbAutoLockPtr<T> & operator=(SHORT const NewHandle)
		{ 
			if (NewHandle != fHandle)
			{
#if defined (MEMORY_CHK)
				if (NewHandle < 0 || abBlockAttr[NewHandle] == UNUSED)
				{
					fatal_error("Handle (%hd) had not been allocated.\n", NewHandle);
				}
				
				if (abBlockAttr[NewHandle] == FREEMEM)
				{
					fatal_error("Handle (%hd) has been free'd.\n", NewHandle);
				}
				
				// Data is on 4 byte boundries.
				// GWP if (GetDataBlkSize(NewHandle) != sizeof(T))
			   // Some pointers are not fixed size.
				// GWP if (GetDataBlkSize(fHandle) != (sizeof(T) + ((sizeof(T) % sizeof(int)) ? sizeof(int) - (sizeof(T)%sizeof(int)) : 0)))
				// GWP {
				// GWP  	fatal_error("Mis-Matched Handle (%hd) size (%ld), and sizeof(%ld).\n",
				// GWP  				NewHandle,GetDataBlkSize(NewHandle), sizeof(T));
				// GWP }
#endif
				if (!fLockFlag)
				{
					ClrLock(fHandle);
				}
				
				fHandle = NewHandle;
				fLockFlag = IsLocked(fHandle);
				if (!fLockFlag)
				{
					SetQuickLock(fHandle);
				}
				fPtr = (T *)BLKPTR(fHandle);
			}
			return *this;	
		}
		
	DumbAutoLockPtr<T> & operator=(DumbAutoLockPtr<T> const &NewDumbHandle)
		{ 
			if (this != &NewDumbHandle)
			{
#if defined (MEMORY_CHK)
				if (NewDumbHandle.fHandle < 0 || abBlockAttr[NewDumbHandle.fHandle] == UNUSED)
				{
					fatal_error("Handle (%hd) had not been allocated.\n", 
															NewDumbHandle.fHandle);
				}
				
				if (abBlockAttr[NewDumbHandle.fHandle] == FREEMEM)
				{
					fatal_error("Handle (%hd) has been free'd.\n", 
														NewDumbHandle.fHandle);
				}
				
				// Data is on 4 byte boundries.
				// GWP if (GetDataBlkSize(NewDumbHandle.fHandle) != sizeof(T))
			   // Some pointers are not fixed size.
				// GWP if (GetDataBlkSize(fHandle) != (sizeof(T) + ((sizeof(T) % sizeof(int)) ? sizeof(int) - (sizeof(T)%sizeof(int)) : 0)))
				// GWP {
				// GWP 	fatal_error("Mis-Matched Handle (%hd) size (%ld), and sizeof(%ld).\n",
				// GWP 				NewDumbHandle.fHandle,
				// GWP 				GetDataBlkSize(NewDumbHandle.fHandle), sizeof(T));
				// GWP }
#endif
				if (!fLockFlag)
				{
					ClrLock(fHandle);
				}
				fHandle = NewDumbHandle.fHandle;	
				// By rechecking we avoid the case where both AutoLock's are useing
				// the same handle.
				fLockFlag = IsLocked(fHandle);
				if (!fLockFlag)
				{
					SetQuickLock(fHandle);
				}
				fPtr = (T *)BLKPTR(fHandle);
			}
			return *this;
		}

	T& operator*() const
		{ return (*fPtr); }
	
	SHORT const mfGetHandle() const
		{ return fHandle; }
		
	operator T*() const
		{ return (fPtr); }

protected:
private:
	T  	* 	fPtr;
	BOOL	fLockFlag;
	SHORT	fHandle;
};

// A template fn to get a data array from a handle with locking.
// Note: Its called "Dumb" because it does no reference counting.

template <class T> class DumbAutoLockArray
{
public:
	DumbAutoLockArray(SHORT const iHandle)
		: fHandle(iHandle),
		    fLockFlag(IsLocked(iHandle)),
		    fPtr((T *)BLKPTR(iHandle))
	{ 
#if defined (MEMORY_CHK)
		if (iHandle < 0 || abBlockAttr[iHandle] == UNUSED)
		{
			fatal_error("Handle (%hd) had not been allocated.\n", iHandle);
		}
		
		if (abBlockAttr[iHandle] == FREEMEM)
		{
			fatal_error("Handle (%hd) has been free'd.\n", iHandle);
		}
#endif
		if (!fLockFlag)
		{
			SetQuickLock(iHandle);		// Lock the memory in place
		}
	}
	
	~DumbAutoLockArray()
	{
		if (!fLockFlag)
		{
			ClrLock(fHandle);
		}
	}
	
	
	// index operators
	T & operator[](LONG const index)
	{ 
#if defined (MEMORY_CHK)
		if (index < 0 || index >= (GetDataBlkSize(fHandle)/sizeof(T)))
		{
			fatal_error("ERROR! Index %ld, Outside array bounds %ld from BLKPTR.\n",
								index, (GetDataBlkSize(fHandle)/sizeof(T)));
		}
#endif
		return (fPtr[index]);
 	}
	
	// for const objects.
	T & operator[](LONG const index) const
	{ 
#if defined (MEMORY_CHK)
		if (index < 0 || index >= (GetDataBlkSize(fHandle)/sizeof(T)))
		{
			fatal_error("ERROR! Index %ld, Outside array bounds %ld from BLKPTR.\n",
								index, (GetDataBlkSize(fHandle)/sizeof(T)));
		}
#endif
		return (fPtr[index]);
 	}
	
	DumbAutoLockArray<T> & operator=(SHORT const NewHandle)
		{ 
#if defined (MEMORY_CHK)
			if (NewHandle < 0 || abBlockAttr[NewHandle] == UNUSED)
			{
				fatal_error("Handle (%hd) had not been allocated.\n", NewHandle);
			}
			
			if (abBlockAttr[NewHandle] == FREEMEM)
			{
				fatal_error("Handle (%hd) has been free'd.\n", NewHandle);
			}
#endif
			if (!fLockFlag)
			{
				ClrLock(fHandle);
			}
			fHandle = NewHandle; 
			fLockFlag = IsLocked(fHandle);
			if (!fLockFlag)
			{
				SetQuickLock(fHandle);
			}
			fPtr = (T *)BLKPTR(fHandle);
			return *this;	
		}
		
	DumbAutoLockArray<T> & operator=(DumbAutoLockArray<T> const &NewDumbHandle)
		{ 
			if (this != &NewDumbHandle)
			{
#if defined (MEMORY_CHK)
				if (NewDumbHandle.fHandle < 0 || abBlockAttr[NewDumbHandle.fHandle] == UNUSED)
				{
					fatal_error("Handle (%hd) had not been allocated.\n", 
											NewDumbHandle.fHandle);
				}
				
				if (abBlockAttr[NewDumbHandle.fHandle] == FREEMEM)
				{
					fatal_error("Handle (%hd) has been free'd.\n", NewDumbHandle.fHandle);
				}
#endif
				if (!fLockFlag)
				{
					ClrLock(fHandle);
				}
				fHandle = NewDumbHandle.fHandle;	
				fLockFlag = IsLocked(fHandle);
				if (!fLockFlag)
				{
					SetQuickLock(fHandle);
				}
				fPtr = (T *)BLKPTR(fHandle);
			}
			return *this;
		}
		
protected:
private:
	T  	* 	fPtr;
	BOOL	fLockFlag;
	SHORT	fHandle;
};
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Inline functions   
   ------------------------------------------------------------------------ */
   
#endif // _HANDLE_HXX
