//--------------------------------------------------------------------------------
//
//		Copyright 1995/1996 by NW Synergistic Software, Inc.
//
//		Registry.c  - Provides registry routines.  Application-specific 
//							items are coded in RegistrS.c
//
//--------------------------------------------------------------------------------

#include <windows.h>
#include "Registry.h"

#define VAR_DWORD  1  // ( unsigned long )
#define VAR_BOOL   2  // ( int ) 
#define VAR_USHORT 3  // ( unsigned short )
#define VAR_SZ     4  // ( string )
extern 	BOOL fAIAutoRes;
typedef struct
{
	char		szName[20];		//---- Name of registry varible
   DWORD		dwType;		   //---- Type of registry varible
   LPVOID	lpVar;         //---- Pointer to actual varible
   DWORD		dwVarType;     //---- Type of variable ( not used for strings ) 
   int		iLen;          //---- Length of variable ( required for strings only)  
   DWORD		dwInit;        //---- Initializer ( use to set or fill ) 
	BOOL		bOption;			//---- True if we should look at save options
									//---- on exit before saving, False means always save
} REGENTRY, * LPREGENTRY;

 // -- Get Application-Specific items
#include "RegistrS.h"




//------------------------------------------------------------------------------------
// Function:   OpenRegistry() 
//
// Inputs:  
//
// Outputs: 
//
// Abstract: Open registry and initialize if it does not exist
//
//------------------------------------------------------------------------------------
BOOL OpenRegistry( PHKEY phKey )
{
	LONG  lReturn;
	DWORD dwDisp;

	lReturn = RegCreateKeyEx( HKEY_LOCAL_MACHINE,
		                       szSubKey, 
			                    0,
				                 NULL,                
					              REG_OPTION_NON_VOLATILE,  
						           KEY_ALL_ACCESS,	       
							        NULL,	                   
								     phKey,	                    
									  &dwDisp );                


	 //---- If registry wasn't here before 
	if ( dwDisp == REG_CREATED_NEW_KEY	)
	{
		InitRegistry ( *phKey );
	}
	else
	{
		LoadRegistry( *phKey );
	}

   return ( TRUE );
} //---- End of OpenRegistry()




//------------------------------------------------------------------------------------
// Function:   CloseRegistry() 
//
// Inputs:  
//
// Outputs: 
//
// Abstract: Close registry. NOTE This saves before closing.
//
//------------------------------------------------------------------------------------
BOOL CloseRegistry( HKEY hKey, BOOL fSaveOptions )
{
	LONG lReturn;


	 //---- Save all vars on close 
	SaveRegistry( hKey, fSaveOptions );

	 //---- Close a registry key 
	lReturn = RegCloseKey ( hKey );

	if ( lReturn == ERROR_SUCCESS )
	{
		return ( TRUE );
	}
	else
	{
		return ( FALSE );
	}

}   //---- End of CloseRegistry();




//------------------------------------------------------------------------------------
// Function:   InitRegistry() 
//
// Inputs:  
//
// Outputs: 
//
// Abstract: Initializes all thexder registry values to default ( used on create )
//
//------------------------------------------------------------------------------------
BOOL InitRegistry ( HKEY hKey )
{
	LONG lReturn; 
	int  x;
	DWORD dwVar;

	for ( x = 0; x < MAX_REGTABLE; x++ )
	{
		switch ( RegTable[x].dwType )
		{
			case REG_SZ:
				memset ( RegTable[x].lpVar, RegTable[x].dwInit, RegTable[x].iLen );

				lReturn = RegSetValueEx( hKey,
													RegTable[x].szName,
                                         0,
                                         REG_SZ,
                                         (PUCHAR) RegTable[x].lpVar,
                                         strlen ( (PCHAR) RegTable[x].lpVar ) + 1 );
				break;


			case REG_DWORD: 
                //---- How do we need to convert 
				switch ( RegTable[x].dwVarType )
				{
					case VAR_DWORD:
						(*(LPDWORD)RegTable[x].lpVar) = RegTable[x].dwInit;          
						dwVar = *(LPDWORD)RegTable[x].lpVar;
						break;

					case VAR_BOOL:
						(*(LPBOOL)RegTable[x].lpVar) = (BOOL) RegTable[x].dwInit;          
						dwVar = (DWORD)*(LPBOOL)RegTable[x].lpVar;
						break;

					case VAR_USHORT:
						(*(LPWORD)RegTable[x].lpVar) = (WORD) RegTable[x].dwInit;          
						dwVar = (DWORD)*(LPWORD)RegTable[x].lpVar;
						break;
				}

				lReturn = RegSetValueEx( hKey,
                                         RegTable[x].szName,
                                         0,
                                         REG_DWORD,
                                         (LPBYTE) &dwVar,
                                         sizeof ( DWORD ) );
				break;

			default:
				break;

		}   //---- switch type 

	}   //---- End of for table entries 

	
	if ( lReturn == ERROR_SUCCESS )
	{
		return ( TRUE );
	}
	else
	{
		return ( FALSE );
	}

}   //---- End of InitRegistry()




//------------------------------------------------------------------------------------
// Function:   LoadRegistry() 
//
// Inputs:  
//
// Outputs: 
//
// Abstract: Loads all registry values into their respective globals etc. 
//
//------------------------------------------------------------------------------------
BOOL LoadRegistry ( HKEY hKey )
{
	LONG  lReturn;
	DWORD dwType;
	DWORD dwLength;
	int   x;
	DWORD dwVar;

	for ( x = 0; x < MAX_REGTABLE; x++ )
	{
		switch ( RegTable[x].dwType )
		{
			case REG_SZ:
				dwLength = RegTable[x].iLen;

				 //---- Look up the varible 
				lReturn = RegQueryValueEx( hKey,
                                           RegTable[x].szName,
                                           NULL,
                                           &dwType,
                                           (PUCHAR) RegTable[x].lpVar,
                                           &dwLength ); 

					//---- Not found so initialize  
				if ( lReturn != ERROR_SUCCESS )
				{
					memset ( RegTable[x].lpVar, RegTable[x].dwInit, RegTable[x].iLen );

					lReturn = RegSetValueEx( hKey,
                                             RegTable[x].szName,
                                             0,
                                             REG_SZ,
                                             (PUCHAR) RegTable[x].lpVar,
                                             strlen ( (PCHAR) RegTable[x].lpVar ) + 1 );


				}
			break;


			case REG_DWORD:
				//---- Load the value  
				dwLength = sizeof(DWORD);

				lReturn = RegQueryValueEx( hKey,
                                           RegTable[x].szName,
                                           NULL,
                                           &dwType,
                                           (LPBYTE) &dwVar,
                                           &dwLength ); 

				if ( lReturn == ERROR_SUCCESS )
				{
					switch ( RegTable[x].dwVarType )
					{
						case VAR_DWORD:
							(*(LPDWORD)RegTable[x].lpVar) = dwVar;          
							break;

						case VAR_BOOL:
							(*(LPBOOL)RegTable[x].lpVar) = (BOOL) dwVar;          
							break;

						case VAR_USHORT:
							(*(LPWORD)RegTable[x].lpVar) = (WORD) dwVar;          
							break;
					}

				}


				 //---- Couldn't Find varible so initialize   
				if ( lReturn != ERROR_SUCCESS )
				{
						 //---- How do we need to convert 

					switch ( RegTable[x].dwVarType )
					{
						case VAR_DWORD:
							(*(LPDWORD)RegTable[x].lpVar) = RegTable[x].dwInit;          
							dwVar = *(LPDWORD)RegTable[x].lpVar;
							break;

						case VAR_BOOL:
							(*(LPBOOL)RegTable[x].lpVar) = (BOOL) RegTable[x].dwInit;          
							dwVar = (DWORD)*(LPBOOL)RegTable[x].lpVar;
							break;

						case VAR_USHORT:
							(*(LPWORD)RegTable[x].lpVar) = (WORD) RegTable[x].dwInit;          
							dwVar = (DWORD)*(LPWORD)RegTable[x].lpVar;
							break;
					}

					lReturn = RegSetValueEx( hKey,
                                         RegTable[x].szName,
                                         0,
                                         REG_DWORD,
                                         (LPBYTE) &dwVar,
                                         sizeof ( DWORD ) );

				}   //---- if 

				break;

			default:
				break;
    
		}   //---- switch type

	}   //---- for regtable


    return ( TRUE );

}   //---- End of LoadRegistry()




//------------------------------------------------------------------------------------
// Function:   SaveRegistry() 
//
// Inputs:  
//		hKey
//		fSaveOptions	- used to optionally save registry items per the table
//
// Outputs: 
//
// Abstract: Save all required vars into registry
//
//------------------------------------------------------------------------------------
BOOL SaveRegistry ( HKEY hKey, BOOL fSaveOptions )
{
	LONG lReturn;
	int  x;
	DWORD dwVar;

	for ( x = 0; x < MAX_REGTABLE; x++ )
	{
		// save all non-optional entries, and optional entries if asked to save
		if ( RegTable[x].bOption == FALSE || 
				fSaveOptions )
		{
			switch ( RegTable[x].dwType )
			{
				case REG_SZ:
					lReturn = RegSetValueEx( hKey,
                                         RegTable[x].szName,
                                         0,
                                         REG_SZ,
                                         (PUCHAR) RegTable[x].lpVar,
                                         strlen ( (PCHAR) RegTable[x].lpVar ) + 1 );
					break;

				case REG_DWORD: 
	                //---- How do we need to convert 

					switch ( RegTable[x].dwVarType )
					{
						case VAR_DWORD:
							dwVar = *(LPDWORD)RegTable[x].lpVar;
							break;

						case VAR_BOOL:
							dwVar = (DWORD)*(LPBOOL)RegTable[x].lpVar;
							break;

						case VAR_USHORT:
							dwVar = (DWORD)*(LPWORD)RegTable[x].lpVar;
							break;
					}

					lReturn = RegSetValueEx( hKey,
                                         RegTable[x].szName,
                                         0,
                                         REG_DWORD,
                                         (LPBYTE) &dwVar,
                                         sizeof(DWORD) );
					break;

				default:
					break;

				}   //---- switch type 
		} //---- End of if
	}   //---- End of for table entries 
   

    return ( TRUE );

}   //---- End of SaveRegistry()




//------------------------------------------------------------------------------------
// Function:   DelRegistry() 
//
// Inputs:  
//
// Outputs: 
//
// Abstract: Delete the subkey completely
//
//------------------------------------------------------------------------------------
BOOL DelRegistry ( void )
{
	long lReturn;

	lReturn = RegDeleteKey( HKEY_LOCAL_MACHINE, szSubKey );          

	if ( lReturn == ERROR_SUCCESS )
	{
		return ( TRUE );
	}
	else
	{
		return ( FALSE );
	}


}   //---- End of DelRegistry()



//---- End of Registry.c
