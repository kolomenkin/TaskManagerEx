// Get File Information ( .exe, .dll )
// Based on http://www.cas.unt.edu/~john/getversion.cpp
//
// Thanks John! http://www.cas.unt.edu/~john

#include "stdafx.h"
#include "DllVersion.h"

BOOL GetModuleVersion( LPTSTR lpszModulePath, CString& lpszFileVersion )
{
   TCHAR    lpszVersion[0x200];
	DWORD    vSize;
	DWORD    vLen,langD;
	BOOL     retVal;	
	
	LPVOID version = NULL;
	LPVOID versionInfo = NULL;

   BOOL success = TRUE;
	
   vSize = GetFileVersionInfoSize( lpszModulePath, &vLen );

	if (vSize) 
   {
		versionInfo = malloc( vSize+1 );
		
      if ( GetFileVersionInfo( lpszModulePath, vLen, vSize, versionInfo ) )
		{			
			_stprintf( lpszVersion, _T("\\VarFileInfo\\Translation") );
			
         retVal = VerQueryValue( versionInfo, lpszModulePath, &version, (UINT *)&vLen );						
			
         if ( retVal && vLen == 4 ) 
         {
   			memcpy( &langD, version, 4);			

	   		_stprintf( lpszVersion, 
                  _T("\\StringFileInfo\\%02X%02X%02X%02X\\FileVersion"),
                  (langD & 0xff00) >> 8,
                  langD & 0xff,
                  (langD & 0xff000000) >> 24,
                  (langD & 0xff0000) >> 16);			
		   }
		   else 
            _stprintf( lpszVersion, _T("\\StringFileInfo\\%04X04B0\\FileVersion"), GetUserDefaultLangID() );
		
         retVal = VerQueryValue( versionInfo, lpszVersion, &version, (UINT *)&vLen );
		
         if (!retVal) 
            success = FALSE;
	   }
	   else 
         success = FALSE;
	}
	else 
      success = FALSE;	
	
	if ( success ) 
   	lpszFileVersion = (LPCTSTR)version;
	
   if (versionInfo) 
      free(versionInfo);

   return success;
}

