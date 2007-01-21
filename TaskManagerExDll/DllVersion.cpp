// Get File Information ( .exe, .dll )
// Based on http://www.cas.unt.edu/~john/getversion.cpp
//
// Thanks John! http://www.cas.unt.edu/~john
// 
// Modified and improved by Sergey Kolomenkin (register@tut.by) in march, 2005
//

#include "stdafx.h"
#include "DllVersion.h"
#include "resource.h"
#include "Localization.h"

#pragma comment (lib, "version.lib")

//////////////////////////////////////////////////////////////////////////

struct LANGANDCODEPAGE {
	WORD wLanguage;
	WORD wCodePage;
};

//////////////////////////////////////////////////////////////////////////

int GetLanguagePos( LANGANDCODEPAGE* lpTranslate, DWORD dwBytesLen )
{
	int iBestPos = 0;
	//LCID locale = GetThreadLocale();
	//LANGID DefaultLang = LOWORD( locale );
	//LANGID DefaultLang2 = GetSystemDefaultLangID();
	//LANGID DefaultLang3 = GetUserDefaultLangID();
	//LANGID DefaultLang = GetUserDefaultLangID();
	//LANGID DefaultLang = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL );
	LANGID DefaultLang = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL );
	CString strLandId = LocLoadString( IDS_LANGID );

	_stscanf( (LPCTSTR)strLandId, _T("0x%X"), &DefaultLang );

	BOOL bNative = FALSE;

	int n = dwBytesLen / sizeof(LANGANDCODEPAGE);

	for( int i=0; i<n; i++ )
	{
		if( PRIMARYLANGID(lpTranslate[i].wLanguage) == LANG_ENGLISH && !bNative ) // English: Default
		{
			iBestPos = i;
		}
		if( PRIMARYLANGID(lpTranslate[i].wLanguage) == PRIMARYLANGID(DefaultLang) ) // Native
		{
			bNative = TRUE;
			iBestPos = i;
		}
		if( lpTranslate[i].wLanguage == DefaultLang ) // Exactly matches
		{
			iBestPos = i;
			break;
		}
	}

	return iBestPos;
}

//////////////////////////////////////////////////////////////////////////

BOOL GetModuleVersion( LPCTSTR lpszModulePath, CString& strFileVersion )
{
	TCHAR    szModulePath[MAX_PATH] = _T("");
	TCHAR    lpszVersion[0x200];
	DWORD    vSize;
	DWORD    vLen;
	BOOL     retVal;	

	LANGANDCODEPAGE *lpTranslate;

	LPVOID version = NULL;
	LPVOID versionInfo = NULL;

	BOOL success = TRUE;

	_tcscpy( szModulePath, lpszModulePath );

	vSize = GetFileVersionInfoSize( szModulePath, &vLen );

	if (vSize) 
	{
		versionInfo = malloc( vSize+1 );

		if ( GetFileVersionInfo( szModulePath, vLen, vSize, versionInfo ) )
		{			
			_stprintf( lpszVersion, _T("\\VarFileInfo\\Translation") );

			retVal = VerQueryValue( versionInfo, lpszVersion, (LPVOID*)&lpTranslate, (UINT *)&vLen );						

			if ( retVal && vLen >= sizeof(LANGANDCODEPAGE) ) 
			{
				int iBestLang = GetLanguagePos( lpTranslate, vLen );

				_stprintf( lpszVersion, 
				_T("\\StringFileInfo\\%04X%04X\\FileVersion"),
					lpTranslate[iBestLang].wLanguage,
					lpTranslate[iBestLang].wCodePage );			
			}
			else
			{
				_stprintf( lpszVersion, _T("\\StringFileInfo\\%04X04B0\\FileVersion"), GetUserDefaultLangID() );
				// Why "04B0" (Unicode) ???                    ---^^^^
			}

			retVal = VerQueryValue( versionInfo, lpszVersion, &version, (UINT *)&vLen );

			if (!retVal)
			{
				retVal = VerQueryValue( versionInfo, _T("\\"), &version, (UINT *)&vLen );
				if(!retVal || vLen < sizeof(VS_FIXEDFILEINFO) )
				{
					success = FALSE;
				}
				else
				{
					VS_FIXEDFILEINFO* pFixed = (VS_FIXEDFILEINFO*) version;
					_stprintf( lpszVersion, _T("%d.%d.%d.%d"),
						HIWORD(pFixed->dwFileVersionMS),
						LOWORD(pFixed->dwFileVersionMS),
						HIWORD(pFixed->dwFileVersionLS),
						LOWORD(pFixed->dwFileVersionLS)
						);
					version = lpszVersion;
				}
			}
		}
		else 
			success = FALSE;
	}
	else 
		success = FALSE;	

	if ( success ) 
		strFileVersion = (LPCTSTR)version;

	if (versionInfo) 
		free(versionInfo);

	return success;
}

//////////////////////////////////////////////////////////////////////////

BOOL GetModuleComment( LPCTSTR lpszModulePath, CString& lpszComment )
{
	TCHAR    szModulePath[MAX_PATH] = _T("");
	TCHAR    lpszVersion[0x200];
	DWORD    vSize;
	DWORD    vLen;
	BOOL     retVal;	

	LANGANDCODEPAGE *lpTranslate;

	LPVOID version = NULL;
	LPVOID versionInfo = NULL;

	BOOL success = TRUE;

	_tcscpy( szModulePath, lpszModulePath );

	vSize = GetFileVersionInfoSize( szModulePath, &vLen );

	if (vSize) 
	{
		versionInfo = malloc( vSize+1 );

		if ( GetFileVersionInfo( szModulePath, vLen, vSize, versionInfo ) )
		{			
			_stprintf( lpszVersion, _T("\\VarFileInfo\\Translation") );

			retVal = VerQueryValue( versionInfo, lpszVersion, (LPVOID*)&lpTranslate, (UINT *)&vLen );						

//			__asm{int 3		}
			if ( retVal && vLen >= sizeof(LANGANDCODEPAGE) ) 
			{
				int iBestLang = GetLanguagePos( lpTranslate, vLen );

				_stprintf( lpszVersion, 
				_T("\\StringFileInfo\\%04X%04X\\Comments"),
					lpTranslate[iBestLang].wLanguage,
					lpTranslate[iBestLang].wCodePage );			
			}
			else
			{
				_stprintf( lpszVersion, _T("\\StringFileInfo\\%04X04B0\\Comments"), GetUserDefaultLangID() );
				// Why "04B0" (Unicode) ???                    ---^^^^
			}

			retVal = VerQueryValue( versionInfo, lpszVersion, &version, (UINT *)&vLen );

			if (!retVal)
			{
				success = FALSE;
			}
		}
		else 
			success = FALSE;
	}
	else 
		success = FALSE;	

	if ( success ) 
		lpszComment = (LPCTSTR)version;

	if (versionInfo) 
		free(versionInfo);

	return success;
}

//////////////////////////////////////////////////////////////////////////

BOOL GetModuleProductVersion( LPCTSTR lpszModulePath, CString& strProductVersion )
{
	TCHAR    szModulePath[MAX_PATH] = _T("");
	TCHAR    lpszVersion[0x200];
	DWORD    vSize;
	DWORD    vLen;
	BOOL     retVal;	

	LANGANDCODEPAGE *lpTranslate;

	LPVOID version = NULL;
	LPVOID versionInfo = NULL;

	BOOL success = TRUE;

	_tcscpy( szModulePath, lpszModulePath );

	vSize = GetFileVersionInfoSize( szModulePath, &vLen );

	if (vSize) 
	{
		versionInfo = malloc( vSize+1 );

		if ( GetFileVersionInfo( szModulePath, vLen, vSize, versionInfo ) )
		{			
			_stprintf( lpszVersion, _T("\\VarFileInfo\\Translation") );

			retVal = VerQueryValue( versionInfo, lpszVersion, (LPVOID*)&lpTranslate, (UINT *)&vLen );						

			if ( retVal && vLen >= sizeof(LANGANDCODEPAGE) ) 
			{
				int iBestLang = GetLanguagePos( lpTranslate, vLen );

				_stprintf( lpszVersion, 
				_T("\\StringFileInfo\\%04X%04X\\ProductVersion"),
					lpTranslate[iBestLang].wLanguage,
					lpTranslate[iBestLang].wCodePage );			
			}
			else
			{
				_stprintf( lpszVersion, _T("\\StringFileInfo\\%04X04B0\\ProductVersion"), GetUserDefaultLangID() );
				// Why "04B0" (Unicode) ???                    ---^^^^
			}

			retVal = VerQueryValue( versionInfo, lpszVersion, &version, (UINT *)&vLen );

			if (!retVal)
			{
				retVal = VerQueryValue( versionInfo, _T("\\"), &version, (UINT *)&vLen );
				if(!retVal || vLen < sizeof(VS_FIXEDFILEINFO) )
				{
					success = FALSE;
				}
				else
				{
					VS_FIXEDFILEINFO* pFixed = (VS_FIXEDFILEINFO*) version;
					_stprintf( lpszVersion, _T("%d.%d.%d.%d"),
						HIWORD(pFixed->dwProductVersionMS),
						LOWORD(pFixed->dwProductVersionMS),
						HIWORD(pFixed->dwProductVersionLS),
						LOWORD(pFixed->dwProductVersionLS)
						);
					version = lpszVersion;
				}
			}
		}
		else 
			success = FALSE;
	}
	else 
		success = FALSE;	

	if ( success ) 
		strProductVersion = (LPCTSTR)version;

	if (versionInfo) 
		free(versionInfo);

	return success;
}

//////////////////////////////////////////////////////////////////////////
