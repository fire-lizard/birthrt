#ifdef _DEBUG
#include "mono.h"

MonoDevice::MonoDevice()
{
   fhDevice = CreateFile("\\\\.\\DARKMONO.VXD", 0, 0, NULL, 0,
        FILE_FLAG_DELETE_ON_CLOSE, NULL);
}   

MonoDevice::~MonoDevice()
{
   if( Status() )
      CloseHandle(fhDevice);
}

#endif
