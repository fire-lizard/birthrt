#ifdef _DEBUG
#ifndef _MONO_H_
#define _MONO_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
// #include <types.h>

//#ifdef __BORLANDC__
//#pragma option -a4
//#endif
//#ifdef _MSC_VER
//#pragma pack(push,4)
//#endif

enum MonoAttrib
{
      MONO_ERASE     =0x00, MONO_NORM     =0x07, MONO_NORM_BLNK     =0x87,
      MONO_INVRS     =0x70, MONO_UNDRLN   =0x01, MONO_UNDRLN_BLNK   =0x81,
      MONO_INVRS_BLNK=0xF0, MONO_HI       =0x0F, MONO_HI_BLNK       =0x8F, 
                            MONO_HI_UNDRLN=0x09, MONO_HI_UNDRLN_BLNK=0x89
};

class MonoDevice
{
private:
   HANDLE fhDevice;
   int  fEnabled;
   char IoBuffer[256];
   enum MonoFunctions 
      {
      MONO_VERSION = 1, 
      MONO_CURSOR_ON = 2, 
      MONO_CURSOR_OFF = 3,
      MONO_SET_ATTRIBUTE = 4,
      MONO_CLEAR_SCREEN = 5,
      MONO_PUT_CHAR = 6,
      MONO_PUT_STRING = 7
      };
public:
   MonoDevice();
   ~MonoDevice();
   int Status();
   int IsEnabled();
   void Enable();
   void Disable();
   void SetAttribute( MonoAttrib attribute );
   void CursorOn();
   void CursorOff();
   void ClearScreen( char fill );
   void PutChar( int row, int col, char value );
   void PutString( int row, int col, const char *string );
   void Printf( int row, int col, const char *format, ... );
};

inline int MonoDevice::Status()
{
   return fhDevice != INVALID_HANDLE_VALUE;
}

inline int MonoDevice::IsEnabled()
{
   return fEnabled;
}

inline void MonoDevice::Enable()
{
   fEnabled = TRUE;
}

inline void MonoDevice::Disable()
{
   fEnabled = FALSE;
}

inline void MonoDevice::SetAttribute( MonoAttrib attribute )
{
   unsigned char attr = (unsigned char)attribute;
   if( Status() )
      DeviceIoControl(fhDevice, MONO_SET_ATTRIBUTE, 
         &attr, sizeof( attr ), 
         NULL, 0, NULL, NULL);
}

inline void MonoDevice::CursorOn()
{
   if( Status() )
      DeviceIoControl(fhDevice, MONO_CURSOR_ON, 
         NULL, 0, NULL, 0, NULL, NULL);
}

inline void MonoDevice::CursorOff()
{
   if( Status() )
      DeviceIoControl(fhDevice, MONO_CURSOR_OFF, 
         NULL, 0, NULL, 0, NULL, NULL);
}

inline void MonoDevice::ClearScreen( char fill )
{
   if( Status() )
      DeviceIoControl(fhDevice, MONO_CLEAR_SCREEN, 
         &fill, sizeof( fill ), NULL, 0, NULL, NULL);
}

inline void MonoDevice::PutChar( int row, int col, char value )
{
   if( Status() )
      {
      short buff[3];
      buff[0] = short(row);
      buff[1] = short(col);
      buff[2] = value;  // high byte doesn't matter
      DeviceIoControl(fhDevice, MONO_PUT_CHAR, 
         &(buff[0]), sizeof( buff ), NULL, 0, NULL, NULL);
      }
}

inline void MonoDevice::PutString( int row, int col, const char *string )
{
   if( Status() )
      {
      short buff[3 + 41];  // allow 80 character string
      buff[0] = short(row);
      buff[1] = short(col);
      buff[2] = short(strlen( string ));
      if( buff[2] > 80 )
         {
         buff[2] = 80;
         strncpy( (char *)(buff + 3), string, 80 );
         }
      else
         strcpy(  (char *)(buff + 3), string );

      DeviceIoControl(fhDevice, MONO_PUT_STRING, 
         &(buff[0]), sizeof( buff ), NULL, 0, NULL, NULL);
      }
}

inline void MonoDevice::Printf( int row, int col, const char *format, ... )
{
   if( Status() )
      {
      char  strbuf [ 256 ];
      va_list argptr;

      va_start(argptr,format);
      vsprintf(strbuf,format,argptr);
      va_end(argptr);

      PutString( row, col, strbuf );
      }
}

//#ifdef __BORLANDC__
//#pragma option -a.
//#endif
//#ifdef _MSC_VER
//#pragma pack(pop)
//#endif

#endif
#endif
