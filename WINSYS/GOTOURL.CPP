// ========================================================================
// Copyright (c) 1997 Sierra On-line Inc. Synergistic Division
// All Rights Reserved
// ========================================================================
// Filename: GotoUrl.cpp  
// Author:   David Jackson
//
// ========================================================================
// Contains the following general functions:
//
// ======================================================================== */
#include <windows.h>
#include <shellapi.h>
#include <string.h>
#include "GOTOURL.H"

//-----------------------------------------------------------------------------
//
//	GetRegKey()
//
//	Description: Get a registry key 
//
// Parameters: HKEY     - handle to reg key
//             *subkey  - key string 
//             *retdata - data 
//
//	Returns: none
//
//-----------------------------------------------------------------------------

long GetRegKey( HKEY key, char *subkey, char *retdata )
{
    long retval;
    HKEY hkey;

    retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);

    if (retval == ERROR_SUCCESS)
    {
        long datasize = MAX_PATH;
        char data[MAX_PATH];

        RegQueryValue(hkey,NULL,(LPSTR)data,&datasize);

        lstrcpy(retdata,data);
        RegCloseKey(hkey);
    }

    return retval;
}


//-----------------------------------------------------------------------------
//
//	GotoURL()
//
//	Description: Go to a web site 
//
// Parameters: url       - url file 
//             showcmd   - 0 
//
//	Returns: BOOL - true ( it worked ) 
//
//-----------------------------------------------------------------------------

BOOL GotoURL( const char *url, int showcmd )
{
	char key[MAX_PATH + MAX_PATH];
	BOOL retflag = FALSE;

   // if the ShellExecute() fails          



   if ( (long)ShellExecute(NULL,"open",url,NULL,NULL,showcmd) <= 32 )
   {
        // get the .htm regkey and lookup the program 
        if (GetRegKey(HKEY_CLASSES_ROOT,".htm",key) == ERROR_SUCCESS)
        {
            lstrcat(key,"\\shell\\open\\command");

            if (GetRegKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS)
            {
                char *pos;
                pos = strstr(key,"\"%1\"");

                if (pos == NULL)     // if no quotes 
                {
                    // now check for %1, without the quotes 

                    pos = strstr(key,"%1");

                    if (pos == NULL) // if no parameter 
                        pos = key+lstrlen(key)-1;
                    else
                        *pos = '\0';    // remove the parameter 
                }
                else
					 {
                    *pos = '\0';        // remove the parameter 
					 }

                lstrcat(pos," ");
                lstrcat(pos,url);

                if (WinExec(key,showcmd) > 31)
                    retflag = TRUE;
            }

        }

	 }
    else
	 {
        retflag = TRUE;
	 }

    return retflag;

}



//-----------------------------------------------------------------------------
//
//	CheckURL()
//
//	Description: Check if we can do URL's
//
// Parameters: url       - url file 
//
//	Returns: BOOL - true ( it worked ) 
//
//-----------------------------------------------------------------------------

BOOL CheckURL ( const char *url )
{
	char key[MAX_PATH + MAX_PATH];
	char pszTmp[100];

	if ( FindExecutable( url, NULL, pszTmp ) > (HANDLE) 32 )
	{
		return TRUE;
	}


   // get the .htm regkey and lookup the program 
	if (GetRegKey(HKEY_CLASSES_ROOT,".htm",key) == ERROR_SUCCESS)
	{
		lstrcat(key,"\\shell\\open\\command");

		if (GetRegKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS)
		{
			return TRUE; // as close as possible without running 
      }
	}


	return FALSE;

}

