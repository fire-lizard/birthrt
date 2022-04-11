//
//		Copyright 1995/1996 by NW Synergistic Software, Inc.
//
//		MDS.H - Include file for midi streams
//
//------------------------------------------------------------------------------

#ifndef _MDS_H
#define _MDS_H

//
// Returned errors
//
#define MDS_SUCCESS                 (0)
#define MDS_ERR_NOMEM               (1)
#define MDS_ERR_NOFILE              (2)
#define MDS_ERR_BADFILE             (3)
#define MDS_ERR_BADFLAGS            (4)
#define MDS_ERR_MIDIERROR           (5)
#define MDS_ERR_INVALHANDLE         (6)
#define MDS_ERR_BADSTATE            (7)


// One of these flags MUST be set
//
// lpbImage -> string containing filename 
//
#define MDS_F_FILENAME              0x00000001L

// lpbImage -> memory image of file
//
#define MDS_F_MEMORY                0x00000002L

// Loop until told to stop
//
#define MDS_F_LOOP                  0x00000001L

#define FOURCC_MIDS mmioFOURCC('M','I','D','S')
#define FOURCC_fmt  mmioFOURCC('f','m','t',' ')
#define FOURCC_data mmioFOURCC('d','a','t','a')

// Format of structs within a MDS file
//
// 'fmt ' chunk
//
#define MDS_F_NOSTREAMID    0x00000001
typedef struct
{
    DWORD           dwTimeFormat;       // Low word == time format in SMF format
    DWORD           cbMaxBuffer;        // Guaranteed max buffer size
    DWORD           dwFlags;
} MIDSFMT;

// 'data' chunk buffer header
//
typedef struct
{
    DWORD           tkStart;            // Absolute tick offset at start of buffer
    DWORD           cbBuffer;           // Bytes in this buffer
} MIDSBUFFER;

// An image handle points to this
//

#define MDSI_F_RESET        0x00000001L
#define MDSI_F_LOOP         0x00000002L
#define MDSI_F_PAUSED       0x00000004L

#define FOURCC_MDSI         mmioFOURCC('M','D','S','I')

#define V_HIMAGE(x) \
    if (((MDSIMAGE*)x)->fccSig != FOURCC_MDSI) \
        return MDS_ERR_INVALHANDLE;

typedef struct
{
    FOURCC          fccSig;             // MDS image handle signature
    MIDSFMT         fmt;                // MDS file format header
    PBYTE           pbBufferAlloc;      // All MIDIHDR's/buffers
    HMIDISTRM       hms;                // MIDI stream if open
    DWORD           fdwImage;           // Generic flags
    DWORD           cBuffers;           // Total buffers
    DWORD           cBuffersInUse;      // Buffers MMSYSTEM owns right now
} MDSIMAGE;

#define MDS_F_IMAGEFLAGS    (MDS_F_MEMORY|MDS_F_FILENAME)

#endif

// Mds.h