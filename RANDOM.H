/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: random.hxx  -
   Author:   David L Jackson

   ========================================================================

	This file contains the random procedures used by Birthright. 
	In general DOS and WIN95 just use the normal rand() function. 
	In multiplayer we do just alittle bit more by logging each random.

   ======================================================================== */


#ifdef _WINDOWS
#ifdef _DEBUG

#if defined (__cplusplus)
extern "C" {
#endif

unsigned long lograndom( char *FileName, SHORT LineNum );
#define rand() lograndom(__FILE__,__LINE__)

void InitRandomLog(void);
void RandomLogOpen ( void );
void RandomLogClose ( void );

void LogFlush ( void );

#if defined (__cplusplus)
}
#endif

#endif
#endif
