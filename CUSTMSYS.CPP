//------------------------------------------------------------------------------------------
//	@doc AUDLIB	
//
// @module CUSTMSYS.CPP -- Customizable audio support function |
//
//	Date:			 04/11/96
// Created by:  Jay Lee
//
// This file must be included in the build to use the Sierra Audio Library.  You may
// leave it untouched, but this will give you a very unsophisticated implementation of
// resource acquisition and memory management.  Based on your needs, feel free to change
// the implementation of any of the functions, just as long as the function prototypes
// stay as they are.
//------------------------------------------------------------------------------------------

#include <stdio.h>
#include <dos.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <io.h>
//#include <malloc.h>
#include <string.h>
#ifdef _WINDOWS
#include <windows.h>  
//#include <mmsystem.h>	 //if you want to use the code in GetElapsedTicks, bring these back in
extern void mMBox(char *,char *);
DWORD		AudioFeedMSec = 2000;
DWORD		AudioIOBytes = 65535;
DWORD		DSoundMSec = 1000;
//extern HWND hMyWnd;
#endif

#include "system.h"
#include "machine.h"

#include "SOLAUDIO/custmsys.hpp"
#include "SOLAUDIO/audio.hpp"

#include "fileutil.h"
#include "sound.hxx"

//-------------------------------------------------------------------------------
// Input: sample number
// Return: pointer to memory-resident audio sample; or
//         0 if specified sample is not memory-resident
// Note: Code to load the specified sample can be placed in this routine.
//-------------------------------------------------------------------------------
void* GetAdr(int num)
{
	SHORT hWave;
	
	// char	pathName[MaxPath + 1];
	// 
	// sprintf(pathName,"%d.WAV",num);
	// return open(pathName,O_BINARY|O_RDONLY);
	char	pathName[_MAX_PATH];

	if(num < 10000)
		sprintf(pathName,"wave00\\%d.WAV",num);

	else if(num < 20000)
		sprintf(pathName,"wave10\\%d.WAV",num);

	else if(num < 30000)
		sprintf(pathName,"wave20\\%d.WAV",num);

//-------------------------------------------------------------------------------
// Note:	Subdirectory split at number 38400 to reduce the total number
//		files in one subdirectory...ALL the voice cue files are numbered
//		between 32000 - 49999.	Filename 38400.wav is the 1st voice cue
//	        for ELF MALE #1 character.
//-------------------------------------------------------------------------------

	else if(num < 38400)
		sprintf(pathName,"%swave30\\%d.WAV",InstallPath, num);

	else if(num < 50000)
		sprintf(pathName,"%swave384\\%d.WAV",InstallPath, num);
		
	else if(num >= 50000)
		sprintf(pathName,"waveloop\\%d.WAV",num);

		
	hWave = TestAndLoadFromResFile(pathName, FALSE);
	if (hWave == fERROR)
	{
		hWave = 0;
	}
	else
	{
		SetBlockAttr(hWave,LOCKED,LOCKED);
	}
	return (void *)hWave;
}

//-------------------------------------------------------------------------------
// Input: sample number
// The specified sample is complete and its memory footprint can be freed.
// Note: This function will be called (and only called) if GetAdr() for the
//       same sample number returned nonzero.
//-------------------------------------------------------------------------------
void FreeAdr(int num)
{
	num=num;
}

//-------------------------------------------------------------------------------
// Input: sample number
// Return: file handle to open file whose current file pointer is positioned
//         to the start of the audio sample; or
//         -1 if the specified sample is not found
// Note: Code to open and position the file can be placed in this routine.
//       This function will be called (and only called) if GetAdr() for the
//       same sample number returned zero.
//-------------------------------------------------------------------------------
int Open(int num)
{
	// char	pathName[MaxPath + 1];
	// 
	// sprintf(pathName,"%d.WAV",num);
	// return open(pathName,O_BINARY|O_RDONLY);
	char	pathName[_MAX_PATH];
		
	if(num == 499)
		sprintf(pathName,"%d.wav",num);

	else if(num < 10000)
		sprintf(pathName,"wave00\\%d.WAV",num);

	else if(num < 20000)
		sprintf(pathName,"wave10\\%d.WAV",num);

	else if(num < 30000)
		sprintf(pathName,"wave20\\%d.WAV",num);


//-------------------------------------------------------------------------------
// Note:	Subdirectory split at number 38400 to reduce the total number
//		files in one subdirectory...ALL the voice cue files are numbered
//		between 32000 - 49999.	Filename 38400.wav is the 1st voice cue
//	        for ELF MALE #1 character.
//-------------------------------------------------------------------------------


	else if(num < 38400)
		sprintf(pathName,"%swave30\\%d.WAV",InstallPath,num);


	else if(num < 50000)
		sprintf(pathName,"%swave384\\%d.WAV",InstallPath, num);
		
	else if(num >= 50000)
		sprintf(pathName,"waveloop\\%d.WAV",num);

		
	return DiskOpen(pathName);
}

//-------------------------------------------------------------------------------
// Input: file handle
// Note: This function will be called (and only called) if Open() did not
//       return -1.
//-------------------------------------------------------------------------------
int Close(int fd)
{
	return DiskClose(fd);
}

//-------------------------------------------------------------------------------
// Input: file handle, file offset, file origin
// Return: file position
//-------------------------------------------------------------------------------
int LSeek(int fd, long offset, int fromWhere)
{
	return lseek(fd, offset, fromWhere);
}

//-------------------------------------------------------------------------------
// Input: file handle
// Return: length of file
//-------------------------------------------------------------------------------
int FileLength(int fd)
{
	return filelength(fd);
}

//-------------------------------------------------------------------------------
// Input: file handle, destination pointer, #bytes to read
// Return: the number of bytes actually read
//-------------------------------------------------------------------------------
int Read(int fd, void* buf, int n)
{
	return read(fd, buf, n);
}

//-------------------------------------------------------------------------------
// Input: A literal string; a default value
// Output: a numeric value associated with the input string
// Note: Currently the stand-alone audio version returns the default value.
// These literals/default values are currently in use:
// audioSize/65536 dacSize/4096 minSubmits/4 maxSubmits/24 leadSubmits/2
//-------------------------------------------------------------------------------
int GetNumEntry(const char *str, int defVal)
{
	if (str);
	return defVal;
}

//-------------------------------------------------------------------------------
// Input: none
// Output: ticks (60th secs) since the application commenced
// Note: This function needs to be changed onlyu if KernelAudioDuration() will be 
//       used to determine elapsed playback time
//-------------------------------------------------------------------------------
int GetElapsedTicks()
{
// Return the application's elapsed time in 60th seconds
	return 1;

	
// Note: I used the code below for Windows	

//	static int iStartTicks = 0;

//	if (iStartTicks == 0)
//	{
//		iStartTicks = GetTickCount();
//	}

//	return( ((GetTickCount() - iStartTicks)*3) / 50 );
}
//-------------------------------------------------------------------------------
// Input: none
// Output: Name of the DOS audio driver to use.  See driver list with package
//         for valid names
// Note: This function is DOS ONLY - it is irrelevant to WINDOWS
//-------------------------------------------------------------------------------
char* GetAudioDriverName()
{
	FILE *fp;
	LONG fileResult;
	char cpBuffer[40];
	//char result[20];

//	strcpy(result,"DACNONE.drv");
	fp=fopen("resource.cfg", "r");
	
	if(fp != NULL)
	{
		while(TRUE)
		{
			fileResult = GetNextLine(fp, cpBuffer, sizeof(cpBuffer));
		
			if(fileResult == EOF)
				return "DACNONE.DRV";

			if(strncmp(cpBuffer,"audioDrv", 8) == 0)
				break;
		}
		
		if(stricmp(cpBuffer, "audioDrv  = DACBLAST.DRV")== 0)
		{
			fclose(fp);
			return "DACBLAST.DRV";
		}

		else if(stricmp(cpBuffer, "audioDrv  =	DACMSSYS.DRV") == 0)
		{
			fclose(fp);
			return "DACMSSYS.DRV";
		}

		else if(stricmp(cpBuffer, "audioDrv  = DACPRO.DRV")== 0)
		{
			fclose(fp);
			return "DACPRO.DRV";
		}

		else if(stricmp(cpBuffer, "audioDrv  = DACPRO8.DRV") == 0)
		{
			fclose(fp);
			return "DACPRO8.DRV";
		}

		else if(stricmp(cpBuffer, "audioDrv  = DACPRO16.DRV") == 0)
		{
			fclose(fp);
			return "DACPRO16.DRV";
		}

		else if(stricmp(cpBuffer, "audioDrv  = DACRRA.DRV") == 0)
		{
			fclose(fp);
			return "DACRRA.DRV";
		}

		else if(strcmp(cpBuffer, "audioDrv  = DACTHUND.DRV") == 0)
		{
			fclose(fp);
			return "DACTHUND.DRV";
		}
	}
// xxxx change this for the release
	return "DACNONE.DRV";

	//return "DACBLAST.DRV";
}

//-------------------------------------------------------------------------------
// Get method defined which currently uses malloc to allocate memory
// Realloc method defined which currently uses realloc to reallocate memory
// Free method defined which currently uses free to release memory
// Read method defined which uses Read() defined above.
//-------------------------------------------------------------------------------
MemID::MemID() : handle(0)
{
}

MemID::MemID(const MemID& id) : handle(id.handle), len(id.len)
{
}

MemID::MemID(void* h) : handle(h), len(GetDataBlkSize(Query_iResBlock((SHORT)h)))
{
}

#if defined (_WINDOWS)
void MemID::Get(int /* type */, size_t size, int /* resHandle */, int /* flag */)
#else
void MemID::Get(size_t size)
#endif
{
	//type = type;
	//resHandle = resHandle;
	//flag = flag;
	
	//handle = malloc(len = size);
	//const short hMemHandle = NewLockedBlock(size);
	const short hMemHandle = NewBlock(size);
	if (hMemHandle != fERROR)
	{
		SetBlockAttr(hMemHandle, LOCKED, LOCKED);
		handle = (void *)hMemHandle;
		len = size;
	}
	else
	{
		handle = 0;
		len = 0;
	}
}

void MemID::Realloc(size_t size)
{
	//handle = realloc(handle, len = size);
	if (handle)
	{
		Free();
	#ifdef _WINDOWS
		Get(0, size);
	#else
		Get(size);
	#endif
		//SetBlockSize((SHORT)handle, size);
	}
	else
	{
	#ifdef _WINDOWS
		Get(0, size);
	#else
		Get(size);
	#endif
	}
}

void MemID::Free()
{
	// if (handle) 
	// {
	// 	free(handle);
	// 	handle = 0;
	// 	len = 0;
	// }
	if (handle)
	{
		SHORT Thishandle = (SHORT)handle;
		handle = 0;
		
		if (IsResourceHandle(Thishandle))
		{
			ClrLock(Thishandle);
			SetPurge(Thishandle);
		}
		else
		{
			DisposBlock(Thishandle);
		}
	}
	len = 0;
}

int MemID::Read(int fd, size_t ofs, size_t size) const
{
	return ::Read(fd, &(*this)[ofs], size);
}

size_t MemID::Size() const
{
	return handle ? len : 0;
}

void MemID::Critical(Boolean /* b */) const
{
	//b=b;
}

// What is the len of the pointer p????
// Who owns this memory???
// Who allocated it!!!
// GWP 3/27/97
// We are actually getting a handle here, from GetAdr.
void* MemID::operator=(void* p)
{
	if (p == 0)
	{
		len = 0;
	}
	else
	{
		if (IsResourceHandle((SHORT)p))
		{
			SHORT const iHandle = Query_iResBlock((SHORT)p);
			
			if (iHandle > 0)
			{
				len = GetDataBlkSize(iHandle);
			}
			else
			{
				len = 0;
			}
		}
		else
		{
			len = GetDataBlkSize((SHORT)p);
		}
	}
	// IF YOU CALL FREE ON THIS AND ITS NOT A HANDLE YOU ARE DEAD!
	// So don't whine if it happens 'cause I warned you.
	if (handle)
	{
		// so we don't lose memory blocks.
		if (IsResourceHandle((SHORT)handle))
		{
			ClrLock((SHORT)handle);
			SetPurge((SHORT) handle);
		}
		else
		{
			DisposBlock((SHORT) handle);
		}
	}
	return handle = p;
}

void* MemID::operator*() const
{
	//return handle;
	if (handle)
		return BLKPTR((short)handle);
	else
		return 0;
}

char& MemID::operator[](size_t s) const
{
	//return *((char*)handle + s);
#if defined(_DEBUG)
	if (s > len)
	{
		fatal_error("ERROR! CUSTMSYS: access beyond array bounds.\n");
	}
#endif
	return *((char*)BLKPTR((short)handle) + s);
}

long MemID::IsAllocated() const
{
	if (handle)
		return 1;
	else
		return 0;
}
// This operator should be tossed in the garbage! Conversions to a type
// with the result a int happen implicitly when you don't really want them.
// Don't say I didn't warn you.
#if 0
MemID::operator int() const
{
	if (handle)
		return 1;
	else
		return 0;
}
#endif

// Load is only used in DOS, so you can ignore it for Windows
int MemID::Load(char* fileName)
{
	int fd;
	//if ((fd = open(fileName, O_RDONLY | O_BINARY)) == -1)
	if ((fd = DiskOpen(fileName)) == -1)
		return 0;
	int size = FileLength(fd);
#ifdef _WINDOWS
	Get(0,size);
#else
	Get(size);
#endif
	int rc = Read(fd,0,size);
	if (rc != size) {
		Free();
		rc = 0;
	}
	DiskClose(fd);
	return rc;
}

#ifdef _WINDOWS
TimeMgr::TimeMgr()
{
}

int
TimeMgr::GetTickCount(void) const
{
	return GetElapsedTicks();
}
TimeMgr*	timeMgr;

ConfigMgr::ConfigMgr()
{
}

int
ConfigMgr::GetNum(char* name, int tokenNum, int defaultN) const
{
	tokenNum = tokenNum;
	return GetNumEntry(name, defaultN);
}
ConfigMgr*	configMgr;

MsgMgr::MsgMgr()
{
}

void
MsgMgr::Fatal(char* fmt, ...) const
{
	char buf[200];

	va_list args;
	va_start(args, fmt);
	sprintf(buf, fmt, args);
	va_end(args);
#ifdef WINDOWS
	mMBox (buf, "FATAL");
#else
	printf(buf);
#endif
	exit(1);
}
MsgMgr*	msgMgr;

#else
//-------------------------------------------------------------------------------
// Input: error number
// Return: Program must abort
// Note: Currently, three fatal conditions exist:
//             Unsupported wave format
//             Unsupported compression format
//             Windows submission error
//-------------------------------------------------------------------------------
void
Fatal(int code,int num)
{
	switch (code) {
		case BAD_FORMAT:
//			printf("Unrecognized audio format for sample#%u\n",num);
			break;
		case COMPRESSED_WAVE:
//			printf("Can't play compressed waves; sample#%u\n",num);
			break;
		case SUBMIT:
//			printf("Error submitting audio\n");
			break;
		default:
			num=num;
//			printf("Unknown audio error\n");
	}
	exit(1);
}

#endif
