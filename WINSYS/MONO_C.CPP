#ifdef _DEBUG
#include <fcntl.h>
#include <sys\stat.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "MONO_C.H"

static MonoDevice MDA;

//---------------------------------------------------------------------------

int mono_getExist( void )
{
   return MDA.Status() ? TRUE : FALSE;  // Returns TRUE, if device detected
}                                       // else returns FALSE

int mono_getEnable( void )
{
   return MDA.IsEnabled() ? TRUE : FALSE; // Returns ON, if device enabled
}                                         // else returns OFF

void mono_setEnable( int flag )    // flag = TRUE  - use mono device
{                                  // flag = FALSE - don't use 
   if( flag == TRUE )
      MDA.Enable();
   else
      MDA.Disable();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void mono_cls(void)
{
   MDA.ClearScreen( ' ' );
}

extern "C" void mono_c_cls(void)
{
   MDA.ClearScreen( ' ' );
}

//---------------------------------------------------------------------------

void mono_putc( short x, short y, char c)
{
   MDA.PutChar( y, x, c );
}

//---------------------------------------------------------------------------

void mono_puts( short x, short y, const char *str )
{
   MDA.PutString( y, x, str );   
}

extern "C" void mono_c_puts( short x, short y, const char *str )
{
   MDA.PutString( y, x, str );   
}

//---------------------------------------------------------------------------

void mono_printf( short x, short y, const char *format, ... )
{
   char  strbuf [ 256 ];
   va_list argptr;
   va_start(argptr,format);
   vsprintf(strbuf,format,argptr);
   va_end(argptr);
   MDA.PutString( y, x, strbuf );
}

extern "C" void mono_c_printf( short x, short y, const char *format, ... )
{
   char  strbuf [ 256 ];
   va_list argptr;
   va_start(argptr,format);
   vsprintf(strbuf,format,argptr);
   va_end(argptr);
   MDA.PutString( y, x, strbuf );
}

//---------------------------------------------------------------------------

void mono_dump  ( short port )
{
}

//---------------------------------------------------------------------------

void mono_setAttrib( MonoAttrib attrib )
{
   MDA.SetAttribute( attrib );
}

MonoAttrib mono_getAttrib()
{
   return MONO_NORM;
}

//---------------------------------------------------------------------------

void  mono_setHardwareCursor( int flag )    // flag = TRUE  - use mono device
{                                            // flag = FALSE - don't use 
   if( flag )
      MDA.CursorOn();
   else
      MDA.CursorOff();
}

#endif
