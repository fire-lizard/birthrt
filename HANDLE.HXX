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

#if !defined(_MEMMANAG_HXX)
#include "memmanag.hxx"
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
		}
		
	T *operator->() const
		{	return ((T *)BLKPTR(fHandle));	}
		
	DumbHandlePtr<T> & operator=(SHORT const NewHandle)
		{ 
			fHandle = NewHandle; 
			return *this;	
		}
		
	DumbHandlePtr<T> & operator=(DumbHandlePtr<T> const &NewDumbHandle)
		{ 
			if (this != &NewDumbHandle)
			{
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
		}
	
	// index operators
	T & operator[](LONG const index)
	{ 
		return (((T *) BLKPTR(fHandle))[index]);
 	}
	
	// for const objects.
	T & operator[](LONG const index) const
	{ 
		return (((T *) BLKPTR(fHandle))[index]);
 	}
	
	DumbHandleArray<T> & operator=(SHORT const NewHandle)
		{ 
			fHandle = NewHandle; 
			return *this;	
		}
		
	DumbHandleArray<T> & operator=(DumbHandleArray<T> const &NewDumbHandle)
		{ 
			if (this != &NewDumbHandle)
			{
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
		return (fPtr[index]);
 	}
	
	// for const objects.
	T & operator[](LONG const index) const
	{ 
		return (fPtr[index]);
 	}
	
	DumbAutoLockArray<T> & operator=(SHORT const NewHandle)
		{ 
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
