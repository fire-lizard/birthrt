//-----------------------------------------------------------------
//
//		Copyright 1995/1996 by NW Synergistic Software, Inc.
//
//		MdsUtil.C - Provides interface for playing mds files.
//
//-----------------------------------------------------------------

#include <Windows.h>
#include <windowsx.h>

#include <mmsystem.h>

#include "MDS.H"

//
// LoadMDSImage
// 
//
DWORD LoadMDSImage(HANDLE *hImage, PBYTE pbImage, DWORD cbImage, DWORD fdw);

//
// FreeMDSImage
//
// Release all memory associated with the given MDS image.
//
// If the given MDS is still playing, it will be stopped and the device
// closed.
//
DWORD FreeMDSImage(HANDLE hImage);
// Play from beginning or last pause state.
//
DWORD PlayMDS(HANDLE hImage, DWORD fdw);

// Pause until next play
//
DWORD PauseMDS(HANDLE hImage);

// Stop. Next play will start at beginning of file again
//
DWORD StopMDS(HANDLE hImage);

DWORD   ParseImage(MDSIMAGE* pImage, PBYTE pbImage, DWORD cbImage);
BOOL    Decompress(LPMIDIHDR lpmhSrc, LPMIDIHDR lpmhDst);

void WINAPI midiCallback( HMIDISTRM hms, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2 );



// LoadMDSImage
//
// Allocate space for the handle structure MDSImage and get the 
// image into memory if it's in a file. Call ParseImage to
// allocate buffers and parse the image into them.
//
DWORD LoadMDSImage(HANDLE *hImage, PBYTE pbImage, DWORD cbImage, DWORD fdw)
{
    DWORD                       dwRet       = MDS_SUCCESS;
    MDSIMAGE*                   pImage      = NULL;
    BOOL                        fIsMapped   = FALSE;
    HANDLE                      hInFile     = INVALID_HANDLE_VALUE;
    HANDLE                      hInFileMap  = NULL;

    // Must have one of the two image flags
    //
    if ((!(fdw & MDS_F_IMAGEFLAGS)) ||
        ((fdw & MDS_F_IMAGEFLAGS) == MDS_F_IMAGEFLAGS))
    {
        dwRet = MDS_ERR_BADFLAGS;
        goto Load_Cleanup;
    }
    
    // Allocate the handle
    //          
    pImage = (MDSIMAGE*)LocalAlloc(LPTR, sizeof(MDSIMAGE));
    if (!pImage)
    {
        dwRet = MDS_ERR_NOMEM;
        goto Load_Cleanup;
    }

    pImage->fccSig          = FOURCC_MDSI;
    pImage->hms             = NULL;
    pImage->cBuffersInUse   = 0;



    // Read the image if we need to 
    //
    if (!(fdw & MDS_F_MEMORY))
    {
        fIsMapped = TRUE;

        // Try to map the file as an image
        //
        hInFile = CreateFile(
            (LPSTR)pbImage, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, 
            FILE_ATTRIBUTE_NORMAL, NULL);

        pbImage = NULL;
        if (INVALID_HANDLE_VALUE == hInFile)
        {
            dwRet = MDS_ERR_NOFILE;
            goto Load_Cleanup;
        }

        cbImage = GetFileSize(hInFile, NULL);

        hInFileMap = CreateFileMapping(
            hInFile, NULL, PAGE_READONLY, 0, 0, NULL);
        if (NULL == hInFileMap)
        {
            dwRet = MDS_ERR_NOFILE;
            goto Load_Cleanup;
        }

        pbImage = (PBYTE)MapViewOfFile(
            hInFileMap, FILE_MAP_READ, 0, 0, 0);
        if (NULL == pbImage)
        {
            dwRet = MDS_ERR_NOFILE;
            goto Load_Cleanup;
        }
    }

    // pbImage now points to the file image in memory. Attempt to parse it.
    //
    dwRet = ParseImage(pImage, pbImage, cbImage);

Load_Cleanup:
    if (dwRet)
    {
        if (pImage) LocalFree((HLOCAL)pImage);
    }
    else
    {
        *hImage = (HANDLE)pImage;
    }

    if (fIsMapped)
    {
        if (NULL != pbImage)                    UnmapViewOfFile(pbImage);
        if (NULL != hInFileMap)                 CloseHandle(hInFileMap);
        if (INVALID_HANDLE_VALUE != hInFile)    CloseHandle(hInFile);
    }

    return dwRet;
}

// Given the file image, allocate MIDI stream buffers and put the 
// image data into them. The file image will go away when LoadMDSImage
// returns, so this routine must save all important info somewhere off
// the handle structure pImage.
//
DWORD ParseImage(MDSIMAGE* pImage, PBYTE pbImage, DWORD cbImage)
{
    DWORD                       dwRet       = MDS_SUCCESS;
    DWORD                       cbChk;
    DWORD                       idx;
    MIDIHDR                     mhSrc;
    LPMIDIHDR                   lpmh;
    MIDSBUFFER                  mb;

    pImage->pbBufferAlloc = NULL;

    // Parse: RIFF + cbChk (size of rest of file) + MIDS
    //
    if ((cbImage < 2*sizeof(FOURCC) + sizeof(DWORD)) ||
        (FOURCC_RIFF != *(FOURCC*)pbImage) ||
        (FOURCC_MIDS != *(FOURCC*)(pbImage + sizeof(FOURCC) + sizeof(DWORD))))
    {
        dwRet = MDS_ERR_BADFILE;
        goto Parse_Cleanup;
    }

    // Note: can't subtract off size of 'fmt ' FOURCC until we check against
    // cbChk size... it's included in the RIFF chunk, not the header
    //
    cbChk = *(((PDWORD)pbImage)+1);
    pbImage += sizeof(FOURCC) + sizeof(DWORD);
    cbImage -= sizeof(FOURCC) + sizeof(DWORD);

    if (cbImage < cbChk)
    {
        dwRet = MDS_ERR_BADFILE;
        goto Parse_Cleanup;
    }

    pbImage += sizeof(FOURCC);
    cbImage -= sizeof(FOURCC);


    // Should have 'fmt ' chunk first
    //
    if ((cbImage < sizeof(FOURCC) + sizeof(DWORD)) ||
        (FOURCC_fmt != *(FOURCC*)pbImage) ||
        ((cbChk = *(PDWORD)(pbImage + sizeof(FOURCC))) > cbImage) ||
        cbChk < sizeof(pImage->fmt))
    {
        dwRet = MDS_ERR_BADFILE;
        goto Parse_Cleanup;
    }

    pbImage += sizeof(FOURCC) + sizeof(DWORD);
    cbImage -= sizeof(FOURCC) + sizeof(DWORD);

    // Already validated size, copy format chunk
    //
    pImage->fmt = *(MIDSFMT*)pbImage;
    
    pbImage += cbChk;
    cbImage -= cbChk;

    // Should get buffers next
    //
    if ((cbImage < sizeof(FOURCC) + sizeof(DWORD)) ||
        (FOURCC_data != *(FOURCC*)pbImage) ||
        ((cbChk = *(PDWORD)(pbImage + sizeof(FOURCC))) > cbImage) ||
        cbChk < sizeof(DWORD))
    {
        dwRet = MDS_ERR_BADFILE;
        goto Parse_Cleanup;
    }

    pImage->cBuffers = *(PDWORD)(pbImage + sizeof(FOURCC) + sizeof(DWORD));

    pbImage += sizeof(FOURCC) + 2*sizeof(DWORD);
    cbImage -= sizeof(FOURCC) + 2*sizeof(DWORD);

    // Now copy the data out, decompressing if needed
    //

    // Total decompressed size including MIDIHDR's to hold them
    //
    // Allocate as one big block and put them into a buffer list
    //
    cbChk = pImage->cBuffers * (sizeof(MIDIHDR) + pImage->fmt.cbMaxBuffer);

    pImage->pbBufferAlloc = (PBYTE)GlobalAllocPtr( GMEM_MOVEABLE, cbChk );

//    pImage->pbBufferAlloc = (PBYTE)GlobalAllocPtr( GMEM_MOVEABLE | GMEM_SHARE, cbChk );

    if (NULL == pImage->pbBufferAlloc)
    {
        dwRet = MDS_ERR_NOMEM;
        goto Parse_Cleanup;
    }
    
    lpmh = (LPMIDIHDR)(pImage->pbBufferAlloc);

    for (idx = pImage->cBuffers; idx; --idx)
    {
        lpmh->lpData = (LPSTR)(lpmh + 1);
        lpmh->dwBufferLength = pImage->fmt.cbMaxBuffer;
        lpmh->dwFlags = 0;
        lpmh->dwUser = (DWORD)pImage;
        lpmh->lpNext = NULL;

        if (cbImage < sizeof(mb))
        {
            dwRet = MDS_ERR_BADFILE;
            goto Parse_Cleanup;
        }

        mb = *(MIDSBUFFER*)pbImage;
        cbImage -= sizeof(mb);
        pbImage += sizeof(mb);

        if (mb.cbBuffer > pImage->fmt.cbMaxBuffer ||
            mb.cbBuffer > cbImage)
        {
            dwRet = MDS_ERR_BADFILE;
            goto Parse_Cleanup;
        }

        if (!(pImage->fmt.dwFlags & MDS_F_NOSTREAMID))
        {
            lpmh->dwBytesRecorded = mb.cbBuffer;
            hmemcpy(lpmh->lpData, pbImage, mb.cbBuffer);
        }
        else
        {
            mhSrc.lpData = (LPSTR)pbImage;
            mhSrc.dwBufferLength = mhSrc.dwBytesRecorded = mb.cbBuffer;
            if (!Decompress(&mhSrc, lpmh))
            {
                dwRet = MDS_ERR_BADFILE;
                goto Parse_Cleanup;
            }
        }

        cbImage -= mb.cbBuffer;
        pbImage += mb.cbBuffer;

        lpmh = (LPMIDIHDR)(((PBYTE)lpmh) + sizeof(MIDIHDR) + pImage->fmt.cbMaxBuffer);
    }


Parse_Cleanup:

    if (dwRet)
    {
        if (pImage->pbBufferAlloc) {
        GlobalFreePtr(pImage->pbBufferAlloc);
        }
    }
    
    return dwRet;
}

// Compression is simply removing the DWORD of stream ID per event since it's
// 0 most of the time unless some really funky authoring is being done. This 
// will save 1 of 3 DWORD's on a MIDI short event, reducing the file size
// to 2/3 of what it was.
//
BOOL Decompress(LPMIDIHDR lpmhSrc, LPMIDIHDR lpmhDst)
{
    LPDWORD                 lpSrc       = (LPDWORD)lpmhSrc->lpData;
    LPDWORD                 lpDst       = (LPDWORD)lpmhDst->lpData;
    DWORD                   cbSrc       = lpmhSrc->dwBytesRecorded;
    DWORD                   cbDst       = lpmhDst->dwBufferLength;
    DWORD                   cbExtra;

    // Total buffer length must be a DWORD multiple
    //
    if (cbSrc & 3)
        return FALSE;

    // !!! OPTIMIZE THIS LOOP !!!
    //
    while (cbSrc)
    {   
        // Need at least space for delta-t, stream-id, event DWORD
        // 
        if (cbDst < 3 * sizeof(DWORD))
            return FALSE;

                                            
        // Event delta-time
        //
        *lpDst++ = *lpSrc++;                
        cbSrc -= sizeof(DWORD);

        // Any event left?
        //
        if (!cbSrc)
            return FALSE;

        // Stream ID
        *lpDst++ = 0;
        cbDst -= 2*sizeof(DWORD);

        // Now copy the actual event data
        //
        cbExtra = 0;
        if ((*lpSrc) & 0x80000000L)
            cbExtra = (*lpSrc) & 0x00FFFFFFL;

        // Long event length is byte aligned, but data is padded to next DWORD
        // in file. 
        //
        cbExtra = (cbExtra + 3) & ~3;

        // Event DWORD itself
        //
        *lpDst++ = *lpSrc++;
        cbSrc -= sizeof(DWORD);
        cbDst -= sizeof(DWORD);

        // Long event parameter data
        //
        if (cbExtra)
        {
            if (cbExtra > cbSrc || cbExtra > cbDst)
                return FALSE;
            
            hmemcpy(lpDst, lpSrc, cbExtra);
        }

//      assert(0 == (cbExtra % sizeof(DWORD)));

        lpDst += (cbExtra / sizeof(DWORD));
        lpSrc += (cbExtra / sizeof(DWORD));
        cbSrc -= cbExtra;
        cbDst -= cbExtra;
    }

    lpmhDst->dwBytesRecorded = (((LPBYTE)lpDst) - (LPBYTE)(lpmhDst->lpData));

    return TRUE;
}

// FreeMDSImage
//
// Get rid of all resources associated with this handle. Stops playback
// if running.
//
DWORD FreeMDSImage(HANDLE hImage)
{
    MDSIMAGE*                   pImage;

    if ( hImage == NULL )
    {
    return MDS_ERR_BADSTATE;
    }

    V_HIMAGE(hImage);
    pImage = (MDSIMAGE*)hImage;
    
    if (NULL != pImage->hms)
        StopMDS(hImage);

    if (pImage->pbBufferAlloc)
    {
    	GlobalFreePtr(pImage->pbBufferAlloc);
		pImage->pbBufferAlloc = NULL;
	}

    // Toast signature in case free'ing the block doesn't; this will
    // cause future V_HIMAGE's to faile
    //
    pImage->fccSig = FOURCC_data;

    LocalFree((HLOCAL)pImage);

    return MDS_SUCCESS;
}

// PlayMDS
//
// Start playback.
//  Open the device if needed.
//  Send the ready list
//
DWORD PlayMDS(HANDLE hImage, DWORD fdw)
{
    DWORD                       dwRet   = MDS_SUCCESS;
    MDSIMAGE*                   pImage;
    LPMIDIHDR                   lpmh;
    UINT                        uDeviceID;
    BOOL                        fCloseOnFail = FALSE;
    DWORD                       idx;
    MIDIPROPTIMEDIV             mptd;

    if ( hImage == NULL )
    {
    return MDS_ERR_BADSTATE;
    }


    V_HIMAGE(hImage);

    pImage = (MDSIMAGE*)hImage;
    
    if (pImage->hms && !(pImage->fdwImage & MDSI_F_PAUSED))
        return MDS_ERR_BADSTATE;

    if (!(pImage->hms))
    {
        fCloseOnFail = TRUE;

        // Starting from scratch. Try to open the MIDI device
        //
        uDeviceID = MIDI_MAPPER;
        if (MMSYSERR_NOERROR != midiStreamOpen(
            &pImage->hms, 
            &uDeviceID, 
            1, 
            (DWORD)midiCallback, 
            0L, 
            CALLBACK_FUNCTION))
        {
            dwRet = MDS_ERR_MIDIERROR;
            goto Play_Cleanup;
        }

        mptd.cbStruct = sizeof(mptd);
        mptd.dwTimeDiv = pImage->fmt.dwTimeFormat;

        if (MMSYSERR_NOERROR !=midiStreamProperty(
            pImage->hms, (LPBYTE)&mptd, MIDIPROP_SET|MIDIPROP_TIMEDIV))
        {
            dwRet = MDS_ERR_MIDIERROR;
            goto Play_Cleanup;
        }

        // Headers are put back into the ready queue by a midiOutReset on
        // stop, but are not guaranteed to be in correct order. Resend 
        // directly from the allocated chunk-of-all-buffers
        //

//      assert(0 == pImage->cBuffersInUse);

        lpmh = (LPMIDIHDR)(pImage->pbBufferAlloc);
        for (idx = pImage->cBuffers; idx; --idx)
        {
            if (MMSYSERR_NOERROR != midiOutPrepareHeader(
                (HMIDIOUT)pImage->hms, lpmh, sizeof(*lpmh)) ||
                MMSYSERR_NOERROR != midiStreamOut(
                pImage->hms, lpmh, sizeof(*lpmh)))
            {
                dwRet = MDS_ERR_MIDIERROR;
                goto Play_Cleanup;
            }

            ++pImage->cBuffersInUse;

            lpmh = (LPMIDIHDR)(((PBYTE)lpmh) + sizeof(MIDIHDR) + lpmh->dwBufferLength);
        }       
    }

    // Whether we're starting or resuming from paused, just need
    // to restart

    pImage->fdwImage &= ~MDSI_F_LOOP;
    if (fdw & MDS_F_LOOP)
        pImage->fdwImage |= MDSI_F_LOOP;

    pImage->fdwImage &= ~MDSI_F_PAUSED;
    if (MMSYSERR_NOERROR != midiStreamRestart(
        pImage->hms))
    {
        dwRet = MDS_ERR_MIDIERROR;
        goto Play_Cleanup;
    }

Play_Cleanup:

    if (dwRet)
    {
        if (fCloseOnFail && pImage->hms)
            StopMDS(hImage);
    }
    
    return dwRet;
}

// PauseMDS
//
// Pause the stream if it's playing
//
DWORD PauseMDS(HANDLE hImage)
{
    MDSIMAGE*                   pImage;

    if ( hImage == NULL )
    {
    return MDS_ERR_BADSTATE;
    }

    V_HIMAGE(hImage);

    pImage = (MDSIMAGE*)hImage;

    if (NULL == pImage->hms)
        return MDS_ERR_BADSTATE;
    
    if (pImage->fdwImage & MDSI_F_PAUSED)
        return MDS_SUCCESS;

    if (MMSYSERR_NOERROR != midiStreamPause(
        pImage->hms))
        return MDS_ERR_MIDIERROR;

    pImage->fdwImage |= MDSI_F_PAUSED;

    return MDS_SUCCESS;
}

// StopMDS
//
// Stop the stream, reset to the start, close the device
// Do NOT free stream tho
//
DWORD StopMDS(HANDLE hImage)
{
    MDSIMAGE*                   pImage;
    DWORD                       idx;
    LPMIDIHDR                   lpmh;


    if ( hImage == NULL )
    {
    return MDS_ERR_BADSTATE;
    }

    V_HIMAGE(hImage);

    pImage = (MDSIMAGE*)hImage;
        
    if (NULL == pImage->hms)
        return MDS_ERR_BADSTATE;

    // Remove all the buffers from MMSYSTEM
    //
    pImage->fdwImage |= MDSI_F_RESET;
    if (MMSYSERR_NOERROR != midiOutReset(
        (HMIDIOUT)pImage->hms))
    {
        pImage->fdwImage &= ~MDSI_F_RESET;
        return MDS_ERR_MIDIERROR;
    }


    // Unprepare everyone
    //

    lpmh = (LPMIDIHDR)( pImage->pbBufferAlloc );

    for (idx = pImage->cBuffers; idx; --idx)
    {
        midiOutUnprepareHeader((HMIDIOUT)pImage->hms, lpmh, sizeof(*lpmh));
        lpmh = (LPMIDIHDR)(((PBYTE)lpmh) + sizeof(MIDIHDR) + lpmh->dwBufferLength);
    }       
    
    midiStreamClose(pImage->hms);
    pImage->hms = NULL;
    pImage->fdwImage = 0;

    return MDS_SUCCESS;
}


// Callback
//
// Keep things rolling or collect the buffers back in the queue
//
void WINAPI midiCallback(HMIDISTRM hms, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
    MDSIMAGE*                   pImage;
    LPMIDIHDR                   lpmh        = (LPMIDIHDR)dw1;

    if (uMsg != MOM_DONE)
        return;

//  assert(NULL != lpmh);
    pImage = (MDSIMAGE*)lpmh->dwUser;
//  assert(FOURCC_MDSI == pImage->fccSig);

    pImage = (MDSIMAGE*)lpmh->dwUser;

    if ( (pImage->fdwImage & MDSI_F_LOOP) && !(pImage->fdwImage & MDSI_F_RESET) )
        if ( MMSYSERR_NOERROR == midiStreamOut( pImage->hms, lpmh, sizeof(*lpmh)) )
            return;
            
    --pImage->cBuffersInUse;
}



// MdsUtil.c






