/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ======================================================================== */
#ifndef _SLRDEMO_HXX
#define _SLRDEMO_HXX

// JPC: Changed all the */* strings to * /*, because VC4 gets confused by */*.

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <time.h>

#if !defined(_SYSTEM_H)
#include "system.h"
#endif

#if !defined(_CAMRDATA_HXX)
#include "camrdata.hxx"
#endif

#if !defined(_GAMEMAP_HXX)
#include "gamemap.hxx"
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
typedef SHORT HDL_SRDEMO_DATA;

 

class SRDEMO_DATA {
public:
	typedef enum { 
		NO_COMMAND,
		MOVE_CAMERA,
		SWOOP_CAMERA,
		SET_CAMERA,
		SAVE_SCENE,
		LOAD_SCENE,
		RESTORE_SCENE,
		WAIT,			// Wait while the scene runs.
		PLAY_MUSIC,
		PLAY_WAVE,
		STOP_WAVE,
		SET_WAVE_VOLUME,
		DISPLAY_PCX,
		SLEEP,			// Don't go back to the render loop!
		EXIT,
		START_DUMP,
		STOP_DUMP,
		LOAD_PALLET,
		RUN_FLIC,
		SET_HIGH_RES,
		SET_LOW_RES,
		HIDE_CURSOR,
		SHOW_CURSOR
	} COMMAND_TYPE;
	
	typedef enum {
		DUMPON,			   //Dump screen to pcx file
		DUMPOFF			   //Do not dump screen to pcx file
	} DUMP_STATE;      
	
	typedef enum {
		RUNNING,
		DONE,
		NO_MORE_COMMANDS,		// Stop the demo when this is seen. No more commands.
	} COMMAND_STATE;
	
	static DUMP_STATE		  fDmpState; 
	static char   		  fDmpPath[40];

	class CMD_DATA;
	
	class EXIT_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
	protected:
	private:
	};
	
	class DISPLAY_PCX_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
	protected:
	private:
	};
	
	class LOAD_PALLET_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
	protected:
	private:
	};
	
	class RUN_FLIC_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
	protected:
	private:
	};
	
	class SET_HIGH_RES_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
	protected:
	private:
	};
	
	class SET_LOW_RES_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
	protected:
	private:
	};
	
	class HIDE_CURSOR_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
	protected:
	private:
	};
	
	class SHOW_CURSOR_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
	protected:
	private:
	};
	
	class SLEEP_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
		COMMAND_STATE mfDoCommand();
		static void mfCallBackSpace(LONG,LONG);
	protected:
	private:
		clock_t		fStartTime;
		LONG		fSleepTime;
		KEYSTRUCT	fOldSpaceKeyStruct;
	};
	
	class WAIT_CMD {
	public:
		void mfInitVals ();
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
		COMMAND_STATE mfDoCommand();
	protected:
	private:
		
		SHORT	fsWaitTime;
	};
	
	class MOVE_CAMERA_CMD {
	public:
		void mfInitVals ();
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
		COMMAND_STATE mfDoCommand();
		
	protected:
	private:
		
		CAMERA_DATA	fCameraData;
	};
	
	class SWOOP_CAMERA_CMD {
	public:
		void mfInitVals ();
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
		COMMAND_STATE mfDoCommand();
		
	protected:
	private:
		
		CAMERA_DATA	fCameraData;
	};
	
   	class SET_CAMERA_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
		
	protected:
	private:
	};
	
	class LOAD_SCENE_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
		
	protected:
	private:
	};
	
	class SAVE_SCENE_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
		
	protected:
	private:
	};
	
	class RESTORE_SCENE_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
		
	protected:
	private:
	};
	
	class PLAYWAVE_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
		
	protected:
	private:
	};
	
	class STOPWAVE_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
		
	protected:
	private:
	};
	
	class SETWAVEVOLUME_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
		
	protected:
	private:
	};
	
	class PLAYMUSIC_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */);
		
	protected:
	private:
	};
	
	class START_DUMP_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */);
	protected:
	private:
	};

	class STOP_DUMP_CMD {
	public:
		static COMMAND_STATE mfReadData(CMD_DATA * /* fCmdData */, char * /* cpBuffer */ );
	protected:
	private:
	};


	// The current command data.
	class CMD_DATA {
	public:
		HDL_SRDEMO_DATA	  fhSRData;		// Handle to this object. used by callbacks.
		union {
			WAIT_CMD  		 	fWaitCmdData;
			MOVE_CAMERA_CMD 	fMoveCameraCmdData;
			SWOOP_CAMERA_CMD 	fSwoopCameraCmdData;
			SLEEP_CMD			fSleepCmdData;
		};
	};
	
	BOOL mfInitVals(char * /* cpFileName */);
	static void mfCallBackStopTheDemo(LONG, LONG);
	COMMAND_STATE mfGetNextCommand();
	COMMAND_STATE mfDoCommand();
	static HDL_SRDEMO_DATA	mfAllocate();
	void mfDelete();
	void dmpToPath();  //Create PCX file in the desired path

	// Rather than make an access fn this becomes available for the callback 
	// routines
	CMD_DATA		  fCmdData;
	
protected:
private:
	void *operator new (size_t stAllocateBlock);	// Can't use new/delete
	void operator delete (void *p);
	SRDEMO_DATA(const SRDEMO_DATA &);					// No copies
	const SRDEMO_DATA operator=(const SRDEMO_DATA &);	// No assignments
	
	FILE 			  			*ffp;
	KEYSTRUCT		  			fOldEscapeKeyStruct;
	COMMAND_TYPE	 			fCmdType;
	COMMAND_STATE	  			fCmdState;
	GAME_CAMERA::CAMERA_STATE	fOldCameraState;
};

typedef SRDEMO_DATA *PTR_SRDEMO_DATA;
 
/* ------------------------------------------------------------------------
   inline functions   
   ------------------------------------------------------------------------ */
// In place of the constructor call this fn.
inline HDL_SRDEMO_DATA SRDEMO_DATA::mfAllocate()
{
	const HDL_SRDEMO_DATA hSRData = NewBlock(sizeof(SRDEMO_DATA));
	if (hSRData != fERROR)
	{
		const PTR_SRDEMO_DATA pSRData = (const PTR_SRDEMO_DATA) BLKPTR(hSRData);
		pSRData->fCmdData.fhSRData = hSRData;
	}
   	return hSRData;
}

// Initialize the wait clock to zero.
inline void SRDEMO_DATA::WAIT_CMD::mfInitVals() 
{
	fsWaitTime = 0;
}


// Wait until the tick count (Frame rate) has elasped.
// GWP Replace with real time wait??
inline SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::WAIT_CMD::mfDoCommand() 
{
	COMMAND_STATE Result;
	
	if (fsWaitTime >= 0)
	{
		fsWaitTime--;
		Result = RUNNING;
	}
	else
	{
		Result = DONE;
	}
	return Result;
}

// Move camera initalization of member data.
inline void SRDEMO_DATA::MOVE_CAMERA_CMD::mfInitVals () 
{
	fCameraData.mfInitVals();
}

// Check whether the camera has arrived, if not keep waiting.
// GWP Add an infinite loop check for blocked cameras.
inline SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::MOVE_CAMERA_CMD::mfDoCommand() 
{
	return ((TRUE == fCameraData.mfIsAtCameraPosition()) ? DONE : RUNNING );
}

// Move camera initalization of member data.
inline void SRDEMO_DATA::SWOOP_CAMERA_CMD::mfInitVals () 
{
	fCameraData.mfInitVals();
}

// Check whether the camera has arrived, if not keep waiting.
// GWP Add an infinite loop check for blocked cameras.
inline SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::SWOOP_CAMERA_CMD::mfDoCommand() 
{
	return ((TRUE == fCameraData.mfIsAtCameraPosition()) ? DONE : RUNNING );
}

// This is the main command loop.
inline SRDEMO_DATA::COMMAND_STATE SRDEMO_DATA::mfDoCommand() 
{
	if (fCmdState == RUNNING)
	{
		// for multi render commands.
		switch (fCmdType)
		{
		case MOVE_CAMERA:
			fCmdState = fCmdData.fMoveCameraCmdData.mfDoCommand();
			break;
		
		case SWOOP_CAMERA:
			fCmdState = fCmdData.fSwoopCameraCmdData.mfDoCommand();
			break;
		
		case SLEEP:
			fCmdState = fCmdData.fSleepCmdData.mfDoCommand();
			break;
			
		case WAIT:
			fCmdState = fCmdData.fWaitCmdData.mfDoCommand();
			break;
		}
	}
	
	if (fCmdState == DONE)
	{
		// Get as many commands as you can do.
		do
		{
			// Note: Some commands are executed as they are read.
			fCmdState = mfGetNextCommand();
		}
		while (fCmdState == DONE);
	}
	if (fDmpState == DUMPON)
	{
		// Create one pcx file into the desired path.
		dmpToPath();
	}
	 
	return fCmdState;
}
   
#endif // _SLRDEMO_HXX
