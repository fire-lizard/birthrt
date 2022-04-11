// CAM 11 DEC 96	Added member function SetFileProgressCallback and data
//						 member pfnFileProgressFunc 
#ifndef _H_FILEXFER
#define _H_FILEXFER

#include "PNetMod.hxx"

const int NETMSGID_RTS       = 249 ;
const int NETMSGID_OKTS      = 250 ;
const int NETMSGID_FILEDATA  = 251 ;
const int NETMSGID_EOF       = 252 ;
const int NETMSGID_FILESTART = 253 ;

const int kXferRequestReceived = 255 ;

typedef void (*PFNFILECALLBACK)( int iCurrentBlock, int iMaxBlock );

#define DATABLOCKSIZE ( MAX_NETBUFF-sizeof(NETMSG) )

typedef struct {
   DWORD dwFileSize ;     // size in bytes
   DWORD dwBlockcount ;   // size in blocks or packets
} XFERSTARTBLOCK ;


//#ifdef _DEFINE_EXPORT_PLATFORMNETWORKMODULE
//class __declspec( dllexport ) FileXfer : public PlatformNetworkModule
//#else
//class __declspec( dllimport ) FileXfer : public PlatformNetworkModule
//#endif

class FileXfer : public PlatformNetworkModule

{
private:
   XFERSTARTBLOCK  m_sbStartBlock ;
   char            m_cXferBuf[DATABLOCKSIZE] ; // buffer for data to xfer
   DWORD           m_dwXferDelay ;   // delay used in sending data blocks
   HANDLE          m_hFile ;         // windows file handle for file in use
   DWORD           m_dwTo ;          // who we are sending to

	PFNFILECALLBACK pfnFileProgressFunc;

   BOOL StartFileSend(USHORT filetype);
   BOOL SendFileBlock(unsigned int index) ;
   BOOL RequestToSend(DWORD dwWait, USHORT filetype) ;

   BOOL StartFileReceive();
   BOOL ReceiveFileBlock(unsigned int index) ;

	


public:
   FileXfer(GamePrintFunction pPrintFunc, ErrCode *err)  ;
   ~FileXfer() ;

	// -- access stored sync info
	virtual int	 GetSync( int sync, int index );		// retreive a sync
	virtual int	 GetSyncId( int sync, int index );	// retreive a sync id
	virtual void SetSync( void );							// store a sync
	virtual void ResetSync( int sync );					// clear ALL stored syncs

   virtual ErrCode GetMessage(LPNETMSG ptheMessage, DWORD dwWait, 
								const char bRemove, BOOL fAck ) ;
                   // Get a message in to the buffer ptheMessage just like
                   // PlatformNetworkModule::GetMessage.
                   // Additionally return ErrCode kXferRequestReceived
                   // if a request to send file is received.

   virtual BOOL ReceiveFile(DWORD dwFrom, char *fname, DWORD dwWait) ;
                   // will return a unique temporary filename in fname.
                   // fname should be MAX_PATH long.
                   // Caller should clean up the temp file fname.
                   // dwWait is the wait time to receive the start of
                   // transfer from the sender (used only once) .

   virtual BOOL SendFile(DWORD dwTo,char *fname, USHORT filetype, DWORD dwWait) ;
                   // assumes a valid pathname in fname, error checking
                   // should have already been done.
                   // filetype is optional, used to give receiver a clue
                   // about what type of file will be received.
                   // dwWait is the wait time to receive ACK of the
                   // start of transfer from the receiver (used only once).

	virtual void SetFileProgressCallback( PFNFILECALLBACK pfnFunc );
						 // Set teh callback function for file progress messages

   virtual void SetXferDelay(DWORD dwWait);
                   // Give user some control over how much
                   // error checking should be done. Longer
                   // for modem game, less for network etc.
                   // controls the wait for each data block ;
} ;





#endif // _H_FILEXFER

