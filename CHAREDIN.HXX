// ===========================================================================
//
// CHAREDIN.HXX
//
// ===========================================================================

#if !defined (_CHAREDIN_HXX_)
#define _CHAREDIN_HXX_

int SelectInventory (HWND hwnd);
void ReadINVTextData (FILE * fp);
BOOL WriteINVFile (char * szInvFileName);

#endif

