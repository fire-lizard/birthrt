/***************************************************************
   filexfer.cpp
   File transfer class for for use with class PlatformNetworkModule
   and Direct Play.

   Uses packet send and receive defined in PlatformNetworkModule to send
   whole files.

   History
   5-16-96   Created by John McKinnie for Synergistic Software
             for use in FootBall 96 for Dynamix.
             Attemps but no guarentees are made to game independence.

	19 SEP 96	CAM	Added FILE_SHARE_READ to SendFile CreateFile
	11 DEC 96	CAM	Added SetFileProgressCallback and implemented callback
							 in the appropriate transfer loops

****************************************************************/

#ifdef _DEBUG
#define _MULTIMONO    //---- Comment out for no mono
#endif

#include "windows.h"
#include "../system.h"

// #define _DEFINE_EXPORT_PLATFORMNETWORKMODULE // header is for export use
#include "filexfer.hxx"
#include "mulplay.hxx"

#include "mono_c.h"

#ifdef _WINDOWS
#ifdef _DEBUG
extern void RandomLogComment ( char * szString );
extern "C" void RandomLogPrefix ( char * szString );
#endif
#endif

#define iMPT_SYNC	20
int FileSyncTable[iMPS_SYNCEND][MAX_PLAYERS];   //---- our inc table
long FileSyncMsgIDTable[iMPS_SYNCEND][MAX_PLAYERS];   //---- message id table

#define XFER_BLOCKS_PER_OKTS	8

const int iRetry = 10;

//---- Sync Message helper
typedef struct
{
   USHORT      usMsgType;	// what is it?
   BOOL        fMaster;    // from master ? 
   DWORD       dwFrom;     // from id ( 0  master ? )
} FBRHeader, *FPBRHeader;

typedef struct
{
	int	      iSync;                //---- which Sync are we at
} FBRSync, *FPBRSync;


FileXfer::FileXfer(GamePrintFunction pPrintFunc, ErrCode *err) :
                  PlatformNetworkModule(pPrintFunc, err)
{
   m_dwXferDelay = 3000 ;
	pfnFileProgressFunc = NULL;
}

FileXfer::~FileXfer()
{
}




void FileXfer::SetXferDelay(DWORD dwWait)
{ 
	m_dwXferDelay = dwWait ; 
} // FileXfer::SetXferDelay




/* ========================================================================
   Function    - FileXfer
	Description - Transmit a save game file
	Returns     - 
	======================================================================== */

BOOL FileXfer::SendFile(DWORD dwTo, char *fname, USHORT filetype, DWORD dwWait)
{
	
	// first see if we can open file
	if ( (m_hFile = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)) == INVALID_HANDLE_VALUE) 
	{
		DWORD dwErr = GetLastError();
		
		#ifdef _MULTIMONO
		mono_printf ( 0, 3, 
			"FileXfer::SendFile Error %d opening file %s",
			dwErr, fname ) ;
		#endif
		
		return FALSE ;
	}
	
	m_dwTo = dwTo ;
	
	// Get permision to start sending data
	if ( !RequestToSend(dwWait, filetype) ) 
	{
		CloseHandle(m_hFile) ;
		return FALSE ;
	}
	
	// Send the Initial block to set up the data transfer
	if( !StartFileSend(filetype) ) 
	{
		CloseHandle(m_hFile) ;
		
		#ifdef _MULTIMONO
		mono_printf ( 0, 4, 
			"FileXfer::SendFile Error sending Init block" );
		#endif
		
		return FALSE ;
	}
	
	
	// send all blocks
	for (unsigned int lcv = 1 ; lcv <= m_sbStartBlock.dwBlockcount ; lcv++) 
	{
		// update the progress bar
		if ( pfnFileProgressFunc != NULL )
			(pfnFileProgressFunc)( lcv, m_sbStartBlock.dwBlockcount );
		
		// get a new OKTS every so many blocks
		// this is to guarantee that we don't flood the SIGS buffers
		if ( (lcv % XFER_BLOCKS_PER_OKTS) == 0)
		{
			if ( !RequestToSend(dwWait, filetype) ) 
			{
				CloseHandle(m_hFile) ;
				return FALSE ;
			}
	
		}
		
		if( !SendFileBlock(lcv) ) 
		{
			CloseHandle(m_hFile) ;
			
			#ifdef _MULTIMONO
			mono_printf ( 0, 5, 
				"FileXfer::SendFile Error sending a block" );
			#endif
			
			return FALSE ;
			
		}
		
		// -- allow Windows it's time
		Sleep(0);
		
	}
	
	CloseHandle(m_hFile) ;
	
	return TRUE ;
	
}



BOOL FileXfer::RequestToSend(DWORD dwWait, USHORT filetype)
{
   DWORD dwTimeOut ;
   ErrCode Err ;

	// -- by releasing the receiver from it's sync point, I have
	// -- already told them I'm ready to send
   
   // wait for permission to send.  Remove extranious ACKs.
   // Abort if another message type is received.

#ifdef _MULTIMONO
   mono_puts ( 0, 7, "FileXfer::Request to send - waiting for OK to send") ;
#endif

#ifdef _WINDOWS
#ifdef _DEBUG
	{
		char temp[200];
		sprintf ( temp, "FILEXFER - RequestToSend Looking for OKTS.");
		RandomLogComment ( temp );
	}
#endif
#endif
   //GEH dwTimeOut = GetTickCount() + dwWait ;
   dwTimeOut = GetTickCount() + 15000 ;

   do 
	{
      Err = ReceiveMessages(0,kPEEKONLY) ;

      //---- If error is other than no message or got message then leave
      // -- GEH getting CheckSum errors that arn't
      if ( (Err != kMessageNotReceived) && (Err != kNoError) ) 
		{
			#ifdef _DEBUG
			{
			char temp[200];
			sprintf ( temp, "FILEXFER - RequestToSend got error %d from ReceiveMessages.",
				Err );
			RandomLogComment ( temp );
			}
			#endif
			return FALSE ;
		}

		//---- If we have a message then do something with it      
		if ( Err == kNoError )
		{
			#ifdef _DEBUG
			{
				char temp[200];
				sprintf ( temp, "FILEXFER - RequestToSend Found Message %d.",
					lpGenericMsgBuf->MsgId );
				RandomLogComment ( temp );
			}
			#endif
			
			if (lpGenericMsgBuf->MsgId == NETMSGID_OKTS) 
			{
				ReceiveMessages(0,kREMOVEFROMQUE) ;
				#ifdef _DEBUG
				{
					char temp[200];
					sprintf ( temp, "FILEXFER - RequestToSend FOUND for OKTS.");
					RandomLogComment ( temp );
				}
				#endif
				return TRUE ;
			}
			else
			// -- Handle SYNC messages
			if (lpGenericMsgBuf->MsgId == iMPT_SYNC) 
			{
				ReceiveMessages(0,kREMOVEFROMQUE);
				// -- uses global data to store sync info
				SetSync();
			}
			else
			{
				#ifdef _DEBUG
				{
				char temp[200];
				sprintf ( temp, "FILEXFER - RequestToSend got unknown message %d.",
					lpGenericMsgBuf->MsgId );
				RandomLogComment ( temp );
				}
				#endif
				// -- don't know Jack, kill it
				ReceiveMessages(0,kREMOVEFROMQUE);
			}
		}
	
		run_timers();
		Sleep(0);
		
	} while ( dwTimeOut > GetTickCount() ) ;
	
	#ifdef _DEBUG
	{
		char temp[200];
		sprintf ( temp, "FILEXFER - RequestToSend TIMEDOUT after %d looking for OKTS.",
			dwWait );
		RandomLogComment ( temp );
	}
	#endif
	
   // -- If you get here, we have failed this attempt
   return FALSE ;

}


BOOL FileXfer::StartFileSend(USHORT filetype)
{
   ErrCode Err ;
   // file in the file info
   m_sbStartBlock.dwFileSize   = GetFileSize(m_hFile, NULL) ;
   m_sbStartBlock.dwBlockcount =
         (m_sbStartBlock.dwFileSize + DATABLOCKSIZE - 1) / DATABLOCKSIZE ;

   if (kNoError != ( Err =
                     SendMessage( m_dwTo,
                                  NETMSGID_FILESTART,
                                  &m_sbStartBlock,
                                  sizeof(XFERSTARTBLOCK),
                                  filetype,
                                  m_dwXferDelay,
                                  iRetry
                                )
                   )
      )
   {
		#ifdef _DEBUG
		{
		char temp[200];
		sprintf ( temp, "FILEXFER - StartFileSend got error %d.",
			Err );
		RandomLogComment ( temp );
		}
		#endif
      return FALSE ;
   }
   return TRUE ;
}

BOOL FileXfer::SendFileBlock(unsigned int index)
{
   DWORD dwBytesRead = 0 ;

   // set file position to the appropriate place for this block
   SetFilePointer(m_hFile,((index-1)*DATABLOCKSIZE),NULL, FILE_BEGIN) ;

   // fill the xfer buffer
   if ( (ReadFile(m_hFile, m_cXferBuf, DATABLOCKSIZE, &dwBytesRead, NULL) != TRUE) || (dwBytesRead == 0) ) 
	{
//      PrintString("From FileXfer::SendFileBlock a ReadFile Error !") ;
//      MessageBox( NULL, "From FileXfer::SendFileBlock a ReadFile Error !", "FileXfer", MB_OK | MB_ICONEXCLAMATION ) ;
      return FALSE ;
   }

   // if a whole block is not available and this is not the last block, abort
   if ( (dwBytesRead != DATABLOCKSIZE) &&
        (index != m_sbStartBlock.dwBlockcount)
      )
   {
      return FALSE ;
   }

//	PrintString( "Sending a file block" );

   if (kNoError != SendMessage(m_dwTo,
                               NETMSGID_FILEDATA,
                               m_cXferBuf,
                               (USHORT) dwBytesRead,
                               index,
                               m_dwXferDelay,
                               iRetry
                              )
      )
   {
      return FALSE ;
   }
   return TRUE ;
}


BOOL FileXfer::ReceiveFile(DWORD dwFrom, char *fname, DWORD dwWait)
{
   m_dwTo = dwFrom ;

	#ifdef _DEBUG
	{
		char temp[200];
		sprintf ( temp, "FILEXFER - ReceiveFile Sending OKTS to master (%d) %d wait and %d retrys.",
			m_dwTo,
			dwWait,
			iRetry );
		RandomLogComment ( temp );
	}
	#endif
	
   // -- Let the master know that I am ready to receive my data
   if (kNoError != SendMessage(m_dwTo,
                               NETMSGID_OKTS,
                               NULL,
                               0,
                               0,
                               dwWait,
                               iRetry
                              )
      )
   {
		#ifdef _DEBUG
		{
			char temp[200];
			sprintf ( temp, "FILEXFER - ReceiveFile FAILED OKTS.");
			RandomLogComment ( temp );
		}
		#endif
      return FALSE ;
   }

   // Get the file start block.  Fill in m_sbStartBlock.
   if (!StartFileReceive()) 
	{
//      PrintString("from FileXfer::ReceiveFile StartFileReceive() failed") ;
      return FALSE ;
   }

   // Create a temporary file and put the name in fname
   if (!GetTempFileName(".","fx_",0,fname))
	{
//      PrintString("from FileXfer::ReceiveFile GetTempFileName() failed") ;
//      MessageBox( NULL, "from FileXfer::ReceiveFile GetTempFileName() failed", "FileXfer", MB_OK | MB_ICONEXCLAMATION ) ;
#ifdef _WINDOWS
#ifdef _DEBUG
	{
		char temp[200];
		sprintf ( temp, "FILEXFER - ReceiveFile GetTempFileName FAILED.");
		RandomLogComment ( temp );
	}
#endif
#endif
      return FALSE ;
   }

   // Open the file and expand it just to be sure we can
   // write all received data to it.
   if ( (m_hFile = CreateFile(fname,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))  == INVALID_HANDLE_VALUE)
   {
//      PrintString("from FileXfer::ReceiveFile CreateFile() failed") ;
//      MessageBox( NULL, "from FileXfer::ReceiveFile CreateFile() failed", "FileXfer", MB_OK | MB_ICONEXCLAMATION ) ;
#ifdef _WINDOWS
#ifdef _DEBUG
	{
		char temp[200];
		sprintf ( temp, "FILEXFER - ReceiveFile CreateFile FAILED.");
		RandomLogComment ( temp );
	}
#endif
#endif
      DeleteFile(fname) ;
      fname[0] = '\0' ;
      return FALSE ;
   }

   if ( m_sbStartBlock.dwFileSize !=
        SetFilePointer(m_hFile,m_sbStartBlock.dwFileSize,NULL, FILE_BEGIN)
      )
   {
//      PrintString("from FileXfer::ReceiveFile SetFilePointer() failed") ;
//      MessageBox( NULL, "from FileXfer::ReceiveFile SetFilePointer() failed", "FileXfer", MB_OK | MB_ICONEXCLAMATION ) ;
#ifdef _WINDOWS
#ifdef _DEBUG
	{
		char temp[200];
		sprintf ( temp, "FILEXFER - ReceiveFile SetFilePointer FAILED.");
		RandomLogComment ( temp );
	}
#endif
#endif
      CloseHandle(m_hFile) ;
      DeleteFile(fname) ;
      fname[0] = '\0' ;
      return FALSE ;
   }

   if( !SetEndOfFile(m_hFile) ) 
	{
//      PrintString("from FileXfer::ReceiveFile SetEndOfFile() failed") ;
//      MessageBox( NULL, "from FileXfer::ReceiveFile SetEndOfFile() failed", "FileXfer", MB_OK | MB_ICONEXCLAMATION ) ;
      CloseHandle(m_hFile) ;
      DeleteFile(fname) ;
      fname[0] = '\0' ;
      return FALSE ;
   }

   // Get the data blocks
   for (unsigned int lcv = 1 ; lcv <= m_sbStartBlock.dwBlockcount ; lcv++) 
	{
		if ( pfnFileProgressFunc != NULL )
			(pfnFileProgressFunc)( lcv, m_sbStartBlock.dwBlockcount );

		// send OKTS every so many block received
		// this is to guarantee that we don't flood the SIGS buffers
		if ( (lcv % XFER_BLOCKS_PER_OKTS) == 0)
		{
   		// -- Let the master know that I am ready to receive my data
   		if (kNoError != SendMessage(m_dwTo,
   		                            NETMSGID_OKTS,
   		                            NULL,
   		                            0,
   		                            0,
   		                            dwWait,
   		                            iRetry
   		                           )
   		   )
   		{
				#ifdef _DEBUG
				{
					char temp[200];
					sprintf ( temp, "FILEXFER - ReceiveFile FAILED OKTS.");
					RandomLogComment ( temp );
				}
				#endif
   		   return FALSE ;
   		}
   	}
   		
		// get a block
		if( !ReceiveFileBlock(lcv) ) 
		{
			#ifdef _DEBUG
			{
				char temp[200];
				sprintf ( temp, "FILEXFER - ReceiveFile ReceiveFileBlock FAILED.");
				RandomLogComment ( temp );
			}
			#endif
			CloseHandle(m_hFile) ;
			DeleteFile(fname) ;
			fname[0] = '\0' ;
			return FALSE ;
		}

   }
   
	#ifdef _DEBUG
	{
	char temp[200];
	sprintf ( temp, "FILEXFER - ReceiveFile WAS COOL.");
	RandomLogComment ( temp );
	}
	#endif
   CloseHandle(m_hFile) ;
   return TRUE ;
}


BOOL FileXfer::StartFileReceive()
{
	ErrCode Err ;
	DWORD dwTimeOut;

	#ifdef _DEBUG
	{
		char temp[200];
		sprintf ( temp, "FILEXFER - StartFileReceive waiting.");
		RandomLogComment ( temp );
	}
	#endif

   //GEH ugly, but I'll fix later
   dwTimeOut = GetTickCount() + 15000 ;
	do 
	{
		Err = ReceiveMessages(0,kPEEKONLY) ;

		//---- If error is other than no message or got message then leave
		if ( (Err != kMessageNotReceived) && (Err != kNoError) ) 
		{
			#ifdef _DEBUG
			{
			char temp[200];
			sprintf ( temp, "FILEXFER - StartFileReceive got error %d from ReceiveMessages.",
				Err );
			RandomLogComment ( temp );
			}
			#endif
			return FALSE ;
		}

		//---- If we have a message then do something with it      
		if ( Err == kNoError )
		{
			if (lpGenericMsgBuf->MsgId == NETMSGID_FILESTART) 
			{
				#ifdef _DEBUG
				{
				char temp[200];
				sprintf ( temp, "FILEXFER - StartFileReceive got NETMSGID_FILESTART.");
				RandomLogComment ( temp );
				}
				#endif
				ReceiveMessages(0,kREMOVEFROMQUE);
				break;
			}
			else
			// -- Handle SYNC messages
			if (lpGenericMsgBuf->MsgId == iMPT_SYNC) 
			{
				ReceiveMessages(0,kREMOVEFROMQUE);
				// -- uses global data to store sync info
				SetSync();
			}
			else
			{
				// -- don't know Jack, kill it
				#ifdef _DEBUG
				{
				char temp[200];
				sprintf ( temp, "FILEXFER - StartFileReceive got message %d from ReceiveMessages, REMOVING.",
					lpGenericMsgBuf->MsgId );
				RandomLogComment ( temp );
				}
				#endif
				ReceiveMessages(0,kREMOVEFROMQUE);
			}
		}
      run_timers();
      Sleep(0);
	} while ( dwTimeOut > GetTickCount() ) ;
	
   memcpy( &m_sbStartBlock, lpGenericMsgBuf->cData, sizeof(XFERSTARTBLOCK) ) ;
   return TRUE ;
}


BOOL FileXfer::ReceiveFileBlock(unsigned int index)
{
   DWORD dwBytesWritten = 0 ;
   DWORD dwTimeOut;
   ErrCode Err ;

	//GEH dwTimeOut = GetTickCount() + (3*m_dwXferDelay) ;
	dwTimeOut = GetTickCount() + 15000;
	do 
	{
		Err = ReceiveMessages(0,kPEEKONLY) ;

		//---- If error is other than no message or got message then leave
		if ( (Err != kMessageNotReceived) && (Err != kNoError) ) 
		{
			#ifdef _DEBUG
			{
			char temp[200];
			sprintf ( temp, "FILEXFER - ReceiveFileBlock got error %d from ReceiveMessages.",
				Err );
			RandomLogComment ( temp );
			}
			#endif
			return FALSE ;
		}

		//---- If we have a message then do something with it      
		if ( Err == kNoError )
		{
			if (lpGenericMsgBuf->MsgId == NETMSGID_FILEDATA) 
			{
				#ifdef _DEBUG
				{
				char temp[200];
				sprintf ( temp, "FILEXFER - ReceiveFileBlock got NETMSGID_FILEDATA.");
				RandomLogComment ( temp );
				}
				#endif
				ReceiveMessages(0,kREMOVEFROMQUE);
				// check for out of missing block
				if (lpGenericMsgBuf->ulSync > index)
					return FALSE;
				break;
			}
			else
			// -- Handle SYNC messages
			if (lpGenericMsgBuf->MsgId == iMPT_SYNC) 
			{
				#ifdef _DEBUG
				{
				char temp[200];
				sprintf ( temp, "FILEXFER - ReceiveFileBlock got iMPT_SYNC.");
				RandomLogComment ( temp );
				}
				#endif
				ReceiveMessages(0,kREMOVEFROMQUE);
				// -- uses global data to store sync info
				SetSync();
			}
			else
			{
				// -- don't know Jack, kill it
				#ifdef _DEBUG
				{
				char temp[200];
				sprintf ( temp, "FILEXFER - ReceiveFileBlock got unknown message %d.",
					lpGenericMsgBuf->MsgId );
				RandomLogComment ( temp );
				}
				#endif
				ReceiveMessages(0,kREMOVEFROMQUE);
			}
		}
      run_timers();
      Sleep(0);
	} while ( dwTimeOut > GetTickCount() ) ;
	
   // if it is not a full block abort, the last block can be small, however.
   if ( (lpGenericMsgBuf->usDataSize != DATABLOCKSIZE) &&
        (index != m_sbStartBlock.dwBlockcount)
      )
   {
//		PrintString( "ReceiveFileBlock - Block too small" );
      return FALSE ;
   }

   // set file position to the appropriate place for this block
   SetFilePointer(m_hFile,((index-1)*DATABLOCKSIZE),NULL, FILE_BEGIN) ;

   // Write the data
   if ( (WriteFile(m_hFile, lpGenericMsgBuf->cData, lpGenericMsgBuf->usDataSize, &dwBytesWritten, NULL) != TRUE) || (dwBytesWritten == 0) )
   {
//      char errstr[256] ;
//      wsprintf(errstr,"From FileXfer::ReceiveFileBlock a WriteFile Error ! %d bytes written ",dwBytesWritten) ;
//      PrintString(errstr) ;
//      MessageBox( NULL, errstr, "FileXfer", MB_OK | MB_ICONEXCLAMATION ) ;
      return FALSE ;
   }
   return TRUE ;
}


ErrCode
FileXfer::GetMessage(LPNETMSG ptheMessage, DWORD dwWait, const char bRemove, 
											BOOL fAck )
{
	ErrCode ecError = kNoError;

   do 
	{
      ecError = ReceiveMessages( dwWait, bRemove, (fAck ? kACK : kDONTACK) );

      if ( kNoError != ecError )
		{
         return ecError ;
      }
      if (lpGenericMsgBuf->MsgId == NETMSGID_RTS) 
	   {
         ecError = (ErrCode) kXferRequestReceived ;
      }
   } while (lpGenericMsgBuf->MsgId == ACK) ;

   memcpy( ptheMessage, lpGenericMsgBuf, MAX_NETBUFF );

	return ( ecError );

} // PlatformNetworkModule::GetMessage




void FileXfer::SetFileProgressCallback( PFNFILECALLBACK pfnFunc )
{
	pfnFileProgressFunc = pfnFunc;
} // FileXfer::SetFileProgressCallback
	

/* ========================================================================
   Function    - Sync record routines
	Description - save sync points for later digestion by a higher source
	Returns     - 
	======================================================================== */

int FileXfer::GetSync( int sync, int index )
{
 	return FileSyncTable[sync][index];
}

int FileXfer::GetSyncId( int sync, int index )
{
 	return FileSyncMsgIDTable[sync][index];
}

void FileXfer::SetSync( void )
{
	int iSync,i,j; 
	
	FPBRHeader pBRHead = (FPBRHeader) (&(lpGenericMsgBuf->cData[0]));
	FPBRSync pSync = (FPBRSync) (&(lpGenericMsgBuf->cData[0]) + sizeof(FBRHeader));

	for(i=0;i<MAX_PLAYERS;i++)
	{
		if ( dwPlayers[i] == lpGenericMsgBuf->dwPlayerId )
			break;
	}
   
	// -- recognize we are at this sync
	FileSyncTable[pSync->iSync][i] = 1;
		
	// -- set the message ID for the message which set the sync
	FileSyncMsgIDTable[pSync->iSync][i] = lpGenericMsgBuf->ulSync;
	
#ifdef _WINDOWS
#ifdef _DEBUG
	{
		char temp[200];
		sprintf ( temp, "FILEXFER - SetSync sync %d   id %d.",
			pSync->iSync, i);
		RandomLogComment ( temp );
	}
#endif
#endif
}

void FileXfer::ResetSync( int sync )
{
	int i;

	//---- Reset replies for this sync point
	
	for ( i = 0; i < MAX_PLAYERS; i++ )
	{
		FileSyncTable[sync][i] = 0;
		FileSyncMsgIDTable[sync][i] = 0;
	}
}

// FileXfer.cpp
