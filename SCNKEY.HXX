/* ========================================================================
   Copyright (c) 1990,1996      Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _SCNKEY_HXX
#define _SCNKEY_HXX

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_CAMRDATA_HXX)
#include "camrdata.hxx"
#endif

#if !defined(_SCNINFO_HXX)
#include "scninfo.hxx"
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
   Prototypes
   ------------------------------------------------------------------------ */
typedef SHORT HDL_KEY;
	
class KEY {
public:
	void mfCreateKey(char *, HDL_KEY);
	void mfWriteKeyData(FILE *);
	static HDL_KEY mfCreateSetCamera(char * /* Key Id String */,
	                                 LONG /* KeyId */,
	                                 LONG /* X */, 
	                                 LONG /* Y */, 
	                                 LONG /* Z */, 
	                                 LONG /* Angle */,
	                                 LONG /* Pitch */, 
	                                 LONG /* Roll */);
	static HDL_KEY mfCreateMoveCamera(char * /* Key Id String */,
	                                  LONG /* KeyId */,
	                                  LONG /* X */, 
	                                  LONG /* Y */, 
	                                  LONG /* Z */, 
	                                  LONG /* Angle */, 
	                                  LONG /* Pitch */, 
	                                  LONG /* Roll */);
	
	// Note: Don't call this if it isn't a Camera Key!
	void mfChangeCameraLocation(LONG  /* X */, 
		                        LONG  /* Y */, 
		                        LONG  /* Z */, 
		                        LONG  /* Angle */, 
		                        LONG  /* Pitch */, 
		                        LONG  /* Roll */);
	LONG mfGetKeyId();
	PFVLL mfGetPrevFunc();
	LONG mfGetPrevVal();
	void mfInitVals();
	
	// These key action fns get a handle to the key in the argument.
	static void mfCallBackSetCamera(LONG, LONG);
	static void mfCallBackMoveCamera(LONG, LONG);
	static void mfCallBackLoadScene(LONG, LONG);
	static void mfCallBackAttachPlayer(LONG, LONG);
	
protected:
private:
	KEY();
	~KEY();
	void *operator new (size_t stAllocateBlock);	// Can't use new/delete
	void operator delete (void *p);
	KEY(const KEY &);			// No copies
	const KEY operator=(const KEY &);	// No assignments
	
	union {
		CAMERA_DATA			fCameraData;
		SCENE_INFO			fscInfo;
	};
	
	LONG					fKeyId;
	KEYSTRUCT				fOldKeyStruct;
	char fcpKeyFuncName[20];
	char fcpKeyIdStr[15];
};
typedef SHORT HDL_KEYS;
typedef KEY *PTR_KEY;
typedef HDL_KEY *PTR_KEYS;
    
	// Note: Don't call this if it isn't a Camera Key!
/* ========================================================================
   Function    - mfChangeCameraLocation
   Description - 
   Returns     - 
   ======================================================================== */
inline void KEY::mfChangeCameraLocation(LONG  X, 
	                        LONG  Y, 
	                        LONG  Z, 
	                        LONG  Angle, 
	                        LONG  Pitch, 
	                        LONG  Roll)
{
	fCameraData.mfSetCameraData(X,Y,Z,Angle,Pitch,Roll);
}

/* ========================================================================
   Function    - mfGetKeyId
   Description - 
   Returns     - 
   ======================================================================== */
inline LONG KEY::mfGetKeyId() {
	return fKeyId;
}

/* ========================================================================
   Function    - mfGetPrevFunc
   Description - 
   Returns     - 
   ======================================================================== */
inline PFVLL KEY::mfGetPrevFunc() {
	return fOldKeyStruct.func;
}

/* ========================================================================
   Function    - mfGetPrevVal
   Description - 
   Returns     - 
   ======================================================================== */
inline LONG KEY::mfGetPrevVal() {
	return fOldKeyStruct.val;
}

/* ========================================================================
   Function    - mfInitVals
   Description - initialize the member data.
   Returns     - 
   ======================================================================== */
inline void KEY::mfInitVals() {
	fCameraData.mfInitVals();
	fKeyId = NO_KEY;
	fOldKeyStruct.func = 0;
	fOldKeyStruct.val = 0;
	fcpKeyFuncName[0] = 0;
	fcpKeyIdStr[0] = 0;
	
}

#endif // _SCNKEY_HXX
