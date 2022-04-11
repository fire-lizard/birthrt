/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _VECTOR_HXX
#define _VECTOR_HXX

/* ------------------------------------------------------------------------
   Vector.hxx
   
   This is a template class designed to catch runtime errors with arrays.
   It does a check against the array bounds for each access through the [].
   Since out-of-bounds access exhibits undefined behavior I choose to abort
   the program with an error message when _VECTOR_CHK is set at compile time.
   If _VECTOR_CHK is not set, you'll get whatever error you would have gotten
   without this checking.
   
   The error message contains the name of the array, the type, the access
   attempted and the size of the array.
   
   While you can't use this technique in the most time critical parts of a
   program, following the 90/10 rule you can use it in the other 90%
   of the code.
   
   Included is a 2D matrix checking class which will check on both boundries.
   
   Use of Vectors is the same as arrays except for the declarations and 
   calls to sizeof.
   
   WARNING!
   sizeof(foo) != sizeof(vectorFoo);
   
   Use instead:
   sizeof(typeName * SIZE_OF_ARRAY);
   
   =====================================
   Quick guide on how to use Vectors.
   
   C Code:
   Typename VariableName[SIZE_OF_ARRAY] = { Initializer list};
   ie:
   long foo[3] = { 1, 2, 3};
   
   Vector Code which will reduce to the C Code if _VECTOR_CHK isn't set.
   DEFINE_VECTOR_DATA_S(Typename, VariableName, SIZE_OF_ARRAY) = { Initializer list };
   DEFINE_VECTOR_CLASS(Typename, VariableName);
   
   ie:
   DEFINE_VECTOR_DATA_S(long, foo, 3) = { 1, 2, 3};
   DEFINE_VECTOR_CLASS(long, foo);
   
   The include file declaration goes from:
   C Code:
   extern long foo[3];
      or
   extern long foo[];
   
   Vector checking code:
   extern DECL_VECTOR_CLASS_S(long, foo, 3);
      or
   extern DECL_VECTOR_CLASS(long, foo);
   =====================================
   
   Without an initializer list there is a single MACRO which combines the
   two used above to define the data space and the class.
   C Code:
   long foo[3];
   
   Vector checking code:
   DECL_VECTOR_DATA(long, foo, 3);
   
   Use the same as the above example include file specification.
   =====================================
   
   For static variables its nearly the same:
   C Code:
   static long foo[3] = { 1, 2, 3};
   
   Vector checking code:
   static DEFINE_STATIC_VECTOR_DATA_S(long, foo, 3) = { 1, 2, 3 };
   DEFINE_STATIC_VECTOR_CLASS(long, foo);
   
   =====================================
   For using in C++ classes:
   
   C++ Code:
   In the .hxx file.
   class Base {
       long foo[3];
   };
   
   In the .cpp file.
   long Base::foo[3];
   
   Vector checking code:
   In the .hxx file.
   class Base {
       DECL_VECTOR_CLASS_S(long, foo, 3);
   };
   
   In the .cpp file.
   DEFINE_VECTOR_MEMBER_DATA_S(long,Base,foo,3)
   DEFINE_VECTOR_MEMBER_CLASS(long,Base,foo)
   
   =====================================
   For use with static member data in C++ classes its nearly the same,
   just add the static keyword everywhere you would normally use it.
   
   C++ Code:
   In the .hxx file.
   class Base {
       static long foo[3];
   };
   
   In the .cpp file.
   long Base::foo[3];
   
   Vector checking code:
   In the .hxx file.
   class Base {
       static DECL_VECTOR_CLASS_S(long, foo, 3);
   };
   
   In the .cpp file its the same. (No static keyword in either regular C++
   or Vector checked code.)
   DEFINE_VECTOR_MEMBER_DATA_S(long,Base,foo,3)
   DEFINE_VECTOR_MEMBER_CLASS(long,Base,foo)
   
   =====================================
   
   For using with 2D matrixes
   C Code:
   long foo[3][4] = {
   { 1, 2, 3, 4},
   { 1, 2, 3, 4},
   { 1, 2, 3, 4},
   };
   
   In .hxx file
   extern long foo[3][];
       or
   extern long foo[3][4];
   
   Vector checking code:
   DEFINE_MATRIX_DATA_S(long, foo,3,4) = {
   { 1, 2, 3, 4},
   { 1, 2, 3, 4},
   { 1, 2, 3, 4},
   };
   DEFINE_MATRIX_CLASS_S(long, foo,3,4);
   
   In .hxx file
   DECL_MATRIX_CLASS(long, foo, 3);
        or
   DECL_MATRIX_CLASS_S(long, foo, 3, 4);
   
   Note: The initialize list is optional, same as for vectors.
   
   =====================================
   For using with arrays of arrays:
   C Code:
   long foo [] = { 1, 2, 3, -1 };
   long goo [] = { 4, 5, 6, 7, -1 };
   long *who[] = { foo, goo };
   
   Vector checking code:
   DEFINE_VECTOR_DATA(long, foo) = { 1, 2, 3, -1};
   DEFINE_VECTOR_CLASS(long, foo);
   
   DEFINE_VECTOR_DATA(long goo) = { 4, 5, 6, 7, -1};
   DEFINE_VECTOR_CLASS(long goo);
   
   DEFINE_VECTOR_ARRAY_DATA(long, who) = { foo, goo };
   DEFINE_VECTOR_ARRAY_CLASS(long, who);
   
   =====================================
   
   Implementation:
   
   In order not to balloon the data space too badly, I created a smart pointer 
   for each vector, It contains the bounds of the array and the debugging
   information of the name and typename of the data.
   
   Matrix's create a Vector on the stack to get at the second dimension
   of the array, passing in an offset into the data space which acts like
   a single vector.

   =====================================
   ------------------------------------------------------------------------ */
   
/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <string.h>
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_MACHINE_H)
#include "machine.h"
#endif


/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define MAX_TYPENAME	40
#define MAX_VARNAME		40

/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
// Macros for use of the Vector class.
#if defined (_VECTOR_CHK)

#define DECL_VECTOR_DATA(T,n,s)					T _ ## n[s]; DEFINE_VECTOR_CLASS(T,n)

#define DECL_VECTOR_CLASS(T,n)			   		BrVector< T >	n
#define DECL_VECTOR_CLASS_S(T,n,s)		   		BrVector< T >	n
#define DECL_VECTOR_ARRAY_CLASS(T,n)			BrVector< BrVector< T > >	n

#define VECTOR_ARRAY_DATA(T, n)					BrVector< T > _ ## n[]
#define VECTOR_ARRAY_S_DATA_S(T, n, s)			BrVector< T > _ ## n[s]

#define DEFINE_STATIC_VECTOR_DATA(T,n)			T _ ## n[]
#define DEFINE_STATIC_VECTOR_DATA_S(T,n,s)		T _ ## n[s]
#define DEFINE_VECTOR_DATA(T,n)		   	   		static T _ ## n[]
#define DEFINE_VECTOR_DATA_S(T,n,s)		   		static T _ ## n[s]
#define DEFINE_VECTOR_MEMBER_DATA(T,c,n)   		static T _ ## n[]
#define DEFINE_VECTOR_MEMBER_DATA_S(T,c,n,s) 	static T _ ## n[s]


#define DEFINE_VECTOR_CLASS(T,n)		   		BrVector<T> n(_ ## n,sizeof(_ ## n)/sizeof(T), #T, #n)
#define DEFINE_STATIC_VECTOR_CLASS(T,n) 		static BrVector<T> n(_ ## n,sizeof(_ ## n)/sizeof(T), #T, #n)
#define DEFINE_VECTOR_MEMBER_CLASS(T,c,n)		BrVector<T> c ## :: ## n(_ ## n,sizeof(_ ## n)/sizeof(T), #T, #n)
#define DEFINE_VECTOR_ARRAY_CLASS(T, n)			DEFINE_VECTOR_CLASS(BrVector< T >, n)

#else

#define DECL_VECTOR_DATA(T,n,s)					DECL_VECTOR_CLASS_S(T,n,s)

#define DECL_VECTOR_CLASS(T,n)			   		T n[]
#define DECL_VECTOR_CLASS_S(T,n,s)		   		T n[s]

#define VECTOR_ARRAY_DATA(T,n)					T * n[]
#define VECTOR_ARRAY_DATA_S(T,n,s)				T * n[s]

#define DEFINE_STATIC_VECTOR_DATA(T,n)			T n[]
#define DEFINE_STATIC_VECTOR_DATA_S(T,n,s)		T n[s]
#define DEFINE_VECTOR_DATA(T,n)		   	   		T n[]
#define DEFINE_VECTOR_DATA_S(T,n,s)		   		T n[s]
#define DEFINE_VECTOR_MEMBER_DATA(T,c,n)   		T c ## :: ## n[]
#define DEFINE_VECTOR_MEMBER_DATA_S(T,c,n,s)	T c ## :: ## n[s]

#define DEFINE_VECTOR_CLASS(T,n)
#define DEFINE_STATIC_VECTOR_CLASS(T,n)
#define DEFINE_VECTOR_MEMBER_CLASS(T,c,n)
#define DEFINE_VECTOR_ARRAY_CLASS(T, n)


#endif

// Macros for use of the Matrix2D class
#if defined(_VECTOR_CHK)
#define DECL_MATRIX_CLASS(T,n,x)				Matrix2D<T> n
#define DECL_MATRIX_CLASS_S(T,n,x,y)			Matrix2D<T>  n

#define DEFINE_MATRIX_DATA(T,n,x)				static T _ ## n[x][]
#define DEFINE_MATRIX_DATA_S(T,n,x,y)		  	static T _ ## n[x][y]

#define DEFINE_MATRIX_CLASS_S(T,n,x,y)			Matrix2D< T > n(_ ## n, x, y, #T, #n)

#else

#define DECL_MATRIX_CLASS(T,n,x)				T n[x][]
#define DECL_MATRIX_CLASS_S(T,n,x,y)			T n[x][y]

#define DEFINE_MATRIX_DATA(T,n,x)				T n[x][]
#define DEFINE_MATRIX_DATA_S(T,n,x,y)			T n[x][y]

#define DEFINE_MATRIX_CLASS_S(T,n,x,y)

#endif

/* ------------------------------------------------------------------------
   Class Definition
   ------------------------------------------------------------------------ */

// Class to validate that array access is within bounds.
// used mainly for debugging.

// WARNING! sizeof ( BrVector<T> n(100) ) != sizeof ( T n[100] )

template <class T> class BrVector 
{
public:
	// Default constructor.
	BrVector() : 
		fnDim(0), 
		fstart(0) 
	{
#if defined (_VECTOR_CHK)
		fcpTypeName[0] = 0;
		fcpVarName[0] = 0;
#endif
	}
	
	BrVector(T *pData, 
		   LONG const x, 
		   char const * const cpTypeName,
		   char const * const cpVarName
		   ) 
		   : fnDim(x), 
		     fstart(pData) 
    {
#if defined (_VECTOR_CHK)
		strncpy(fcpTypeName, cpTypeName, MAX_TYPENAME - 1);
		fcpTypeName[MAX_TYPENAME - 1] = 0;
		strncpy(fcpVarName, cpVarName, MAX_VARNAME - 1);
		fcpVarName[MAX_VARNAME - 1] = 0;
#endif
    }
	
	// Destructor.
	~BrVector(){};
	
	// index operators
	T& operator[](LONG const index)
	{
#if defined (_VECTOR_CHK)
		if (index < 0 || index >= fnDim)
		{
			fatal_error("ERROR! %s %s[%ld], Access outside array bounds %ld.\n",
						 fcpTypeName, fcpVarName, index, fnDim);
		}
#endif
		return fstart[index];
	}
	
	// for const objects.
	T& operator[](LONG const index) const
	{
#if defined (_VECTOR_CHK)
		if (index < 0 || index >= fnDim)
		{
		    fatal_error("ERROR! Index %s %s[%ld], Access outside array bounds %ld.\n",
		    			fcpTypeName, fcpVarName, index, fnDim);
		}
#endif
		return fstart[index];
	}
	
	// Copy constructor
	BrVector< T > &operator=( BrVector< T > & NewVector ) 
	{
		fnDim = NewVector.fnDim;
		fstart = NewVector.fstart;

#if defined (_VECTOR_CHK)
		strncpy(fcpTypeName, NewVector.fcpTypeName, MAX_TYPENAME);
		strncpy(fcpVarName, NewVector.fcpVarName, MAX_VARNAME);
#endif
		return *this;
	}
	
	
	BrVector < T > & operator= (BrVector < T > * pNewVector)
	{
		fnDim = pNewVector->fnDim;
		fstart = pNewVector->fstart;

#if defined (_VECTOR_CHK)
		strncpy(fcpTypeName, pNewVector->fcpTypeName, MAX_TYPENAME);
		strncpy(fcpVarName, pNewVector->fcpVarName, MAX_VARNAME);
#endif
		return *this;
	}
	
	// So the data can be accessed for writing out.
	// This in general this is a BAD idea. Its only here so things will compile.
	operator char *() 
	{ return ((char *)fstart);	}
	
protected:
	LONG	fnDim;
	
#if defined (_VECTOR_CHK)
	char	fcpTypeName[MAX_TYPENAME];
	char	fcpVarName[MAX_VARNAME];
#endif

	T 	  *fstart;
	
private:
};


// Check in two directions. But create a BrVector<T> on the stack.
template <class T> class Matrix2D
{
public:
	Matrix2D(void * pData, 
			 LONG const r, 
			 LONG const c,
			 char const * const cpTypeName,
			 char const * const cpVarName) :
		fRows(r), 
		fCols(c), 
		start((T *)pData)
	{ 
#if defined (_VECTOR_CHK)
		strncpy(fcpTypeName, cpTypeName, MAX_TYPENAME - 1);
		fcpTypeName[MAX_TYPENAME - 1] = 0;
		strncpy(fcpVarName, cpVarName, MAX_VARNAME - 1);
		fcpVarName[MAX_VARNAME - 1] = 0;
#endif
	}
	
	BrVector<T> operator[] (LONG const index)
	{
#if defined (_VECTOR_CHK)
		if (index < 0 || index >= fRows)
		{
			fatal_error("ERROR! %s %s[%ld], Access outside array bounds %ld.\n",
						 fcpTypeName, fcpVarName, index, fRows);
		}
#endif
		BrVector<T> Result(start + (index*fCols), fCols, fcpTypeName, fcpVarName);
		
		return ( Result );
	}
	
	BrVector<T> operator[] (LONG const index) const	// for const objects.
	{
#if defined (_VECTOR_CHK)
		if (index < 0 || index >= fRows)
		{
			fatal_error("ERROR! %s %s[%ld], Access outside array bounds %ld.\n",
						 fcpTypeName, fcpVarName, index, fRows);
		}
#endif
		BrVector<T> Result(start + (index*fCols), fCols, fcpTypeName, fcpVarName);
		
		return ( Result );
	}
	
	// So the data can be accessed for writing out.
	// This in general this is a BAD idea. Its only here so things will compile.
	operator char *() 
	{ return (char *)start;	}

	
protected:
	LONG	fRows;
	LONG	fCols;
	
#if defined (_VECTOR_CHK)
	char	fcpTypeName[MAX_TYPENAME];
	char	fcpVarName[MAX_VARNAME];
#endif
	
	T		*start;

private:
};

/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Inline functions   
   ------------------------------------------------------------------------ */
   
#endif // _VECTOR_HXX
