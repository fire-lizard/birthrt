#ifdef _DEBUG
#include "MONO.H"

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
