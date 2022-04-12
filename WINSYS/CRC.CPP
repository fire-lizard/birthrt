// -------------------------------------------------------------------
//
//		Crc.cpp
//
//		Crc calculating code
//
//		Programmed by:		Alan B. Clark
//								Synergistic Software
//
// -------------------------------------------------------------------

//#include "Global.h"
#include "Windows.h"

/* ========================================================================
	HashCRC	- Uses polynomial division to hash a string into a word
	======================================================================== */
static USHORT		accumCRC = 0;

BYTE NextCRC (BYTE c)
{
	USHORT		shift, flag;

	for (shift = 0x80; shift; shift >>= 1)
	{
		flag = (USHORT)(accumCRC & 0x8000);
		accumCRC <<= 1;
		accumCRC |= ((shift & c) ? 1 : 0);
		if (flag) 
			accumCRC ^= 0x1021;
	}
	return c;
}

//USHORT HashCRC (CSTRPTR sz)
//{
//	USHORT		i;
//
//	i = 0;
//	accumCRC = 0;
//	while (NextCRC(sz[i++])) ;
//	NextCRC(0);
//	return accumCRC;
//}
USHORT HashCRC ( PUCHAR lpMsg, USHORT usSize )
{
	USHORT		i;

	i = 0;
	accumCRC = 0;
//	while (NextCRC(lpMsg[i++])) ;
	for ( i = 0; i < usSize; i++ )
		NextCRC(lpMsg[i]);
	NextCRC(0);
	return accumCRC;
}

/* ========================================================================
	Use Example
	======================================================================== */

	/* use CRC to hash the name into a USHORT */
//	wNameHash = HashCRC(szResName);


// Crc.cpp