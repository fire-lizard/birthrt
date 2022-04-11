/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: CThing.hxx
   Author:   Craig Clayton & Gary Powell
   ======================================================================== */
#if !defined(_CTHING_HX)
#define _CTHING_HXX

#include <stdio.h>

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_ATHING_HXX)
#include "athing.hxx"
#endif

#if !defined(_GAMETYPE_HXX)
#include "gametype.hxx"
#endif


class CThing : public ArtThing {
public:
	typedef void (*PFVC)(CThing *);
	
	PFVC		RightButtonFn;		// Function to do when the right mouse click
	PFVC		LeftButtonFn;		// Function to do when the left mouse click
	
	inline void mfDefineCThing(THINGTYPE const, 
						LONG const /* x */, 
						LONG const /* y */, 
						LONG const /* z */, 
						LONG const /* a */,
						LONG const h=0,
						PFVC const RightBtnFn = 0,
						PFVC const LeftBtnFn  = 0);

	inline void mfAttachCThing(LONG const /* ThingIndex */,
						THINGTYPE const,
						PFVC const NewRightBtnFn = 0,
						PFVC const NewLeftBtnFn  = 0);
						
	inline PFVC const mfSetRightButtonFn(PFVC const /* NewRightButtonFn */);
	inline PFVC const mfSetLeftButtonFn(PFVC const /* NewLeftButtonFn */);
	inline BOOL const mfIsLeftButtonFnSet() const;
	inline BOOL const mfIsRightButtonFnSet() const;
	
	inline BOOL const mfIsHuman() const;
	inline BOOL const mfIsMonster() const;
	inline BOOL const mfIsAnimal() const;
	
protected:
private:
	// Defined but not implemented on purpose to generate link errors.
	CThing();
	~CThing();
	CThing operator=(const CThing &);
	CThing(const CThing &);
    void *operator new( size_t stAllocateBlock);
    void operator delete( void *p );
};



// This routine initializes the CThing data.
inline void CThing::mfDefineCThing(THINGTYPE const tt, 
								   LONG const x, 
								   LONG const y, 
								   LONG const z, 
								   LONG const a,
								   LONG const , /* h */
								   PFVC const NewRightBtnFn,
								   PFVC const NewLeftBtnFn)
{
	mfDefineArtThing(tt, x, y, z, a);
	RightButtonFn = NewRightBtnFn;
	LeftButtonFn = NewLeftBtnFn;
	
}

// This routine attaches a CThing to an existing thing in the mythings list.
inline void CThing::mfAttachCThing(LONG const ThingIndex,
                                   THINGTYPE const tt,
								   PFVC const RightBtnFn,
								   PFVC const LeftBtnFn)
{
	mfAttachArtThing(ThingIndex, tt);
	RightButtonFn = RightBtnFn;
	LeftButtonFn = LeftBtnFn;
	
}

// Test if this CThing type is a humanoid or not.
inline BOOL const CThing::mfIsHuman() const
{
	return GAME_TTYPE::mfIsHuman(mfType());
}

// Test if this CThing is a monster or not.
inline BOOL const CThing::mfIsMonster() const
{
	return GAME_TTYPE::mfIsMonster(mfType());
}

// Test if this CThing is an Animal or not.
inline const BOOL CThing::mfIsAnimal() const
{
	return GAME_TTYPE::mfIsAnimal(mfType());
}

// Set a new Right button fn for this CThing.
inline CThing::PFVC const CThing::mfSetRightButtonFn(PFVC const NewRightButtonFn)
{
	PFVC const Result = RightButtonFn;
	
	RightButtonFn = NewRightButtonFn;
	
	return Result;
}

// Set a new Left button fn for this CThing.
inline CThing::PFVC const CThing::mfSetLeftButtonFn(PFVC const NewLeftButtonFn)
{
	PFVC const Result = LeftButtonFn;
	
	LeftButtonFn = NewLeftButtonFn;
	
	return Result;
}

// Test whether there is allready a button fn set.
inline BOOL const CThing::mfIsLeftButtonFnSet() const
{
	return (LeftButtonFn != 0);
}

// Test whether there is allready a button fn set.
inline BOOL const CThing::mfIsRightButtonFnSet() const
{
	return (RightButtonFn != 0);
}
#endif // _CTHING_HXX

