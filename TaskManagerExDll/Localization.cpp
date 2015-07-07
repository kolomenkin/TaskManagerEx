//////////////////////////////////////////////////////////////
// File:		// Localization.cpp
// File time:	// 12.04.2005	14:31
// Version: 	// 1.0
// Author:		// Sergey Kolomenkin <register@tut.by>
// Copyright:	// (c) Sergey Kolomenkin, Minsk, 2005
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#include "stdafx.h"
//#include <windows.h>
#include "Localization.h"

//////////////////////////////////////////////////////////////////////////

// #pragmas are used here to insure that the structure's
// packing in memory matches the packing of the EXE or DLL.
#pragma pack( push )
#pragma pack( 2 )
typedef struct
{
   BYTE   bWidth;               // Width, in pixels, of the image
   BYTE   bHeight;              // Height, in pixels, of the image
   BYTE   bColorCount;          // Number of colors in image (0 if >=8bpp)
   BYTE   bReserved;            // Reserved
   WORD   wPlanes;              // Color Planes
   WORD   wBitCount;            // Bits per pixel
   DWORD   dwBytesInRes;         // how many bytes in this resource?
   WORD   nID;                  // the ID
} GRPICONDIRENTRY, *LPGRPICONDIRENTRY;
#pragma pack( pop )

#pragma pack( push )
#pragma pack( 2 )
typedef struct 
{
   WORD            idReserved;   // Reserved (must be 0)
   WORD            idType;       // Resource type (1 for icons)
   WORD            idCount;      // How many images?
   GRPICONDIRENTRY   idEntries[1]; // The entries for each image
} GRPICONDIR, *LPGRPICONDIR;
#pragma pack( pop )

typedef struct
{
   BITMAPINFOHEADER   icHeader;      // DIB header
   RGBQUAD         icColors[1];   // Color table
   BYTE            icXOR[1];      // DIB bits for XOR mask
   BYTE            icAND[1];      // DIB bits for AND mask
} ICONIMAGE, *LPICONIMAGE;

//////////////////////////////////////////////////////////////
/*
// Called as follows: If IDS_TEST is the string you wish to load and
// WCHAR Buffer[128] is your buffer, the call would be
// MyLoadStringW(IDS_TEST,Buffer,128);
// If it succeeds, the function returns the number of characters copied
// into the buffer, not including the NULL terminating character, or
// zero if the string resource does not exist.
int MyLoadStringW(UINT wID, LPWSTR wzBuf, int cchBuf)
{
	UINT    block, num;
	int     len = 0;
	HRSRC   hRC = NULL;
	HGLOBAL hgl = NULL;
	LPWSTR  str = NULL;
	UINT i;

	wzBuf[0] = L'\0';

	block = (wID >> 4) + 1;   // Compute block number.
	num = wID & 0xf;      // Compute offset into block.

	hRC = FindResourceEx(NULL,
			RT_STRING,
			MAKEINTRESOURCE(block),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
	hgl = LoadResource(NULL, hRC);

	str = (LPWSTR)LockResource(hgl);
	if (str)
	{
		for (i = 0; i < num; i++)
		{
			str += *str + 1;
		}
		wcsncpy(wzBuf, str + 1, min(cchBuf - 1, *str));

		wzBuf[min(cchBuf-1, *str) ] = '\0';

		len = *str;
	}
	return len;
}
*/
//////////////////////////////////////////////////////////////

void SetThreadNativeLanguage()
{
#ifdef NATIVE_LANGUAGE
//	LCID locale = GetUserDefaultLCID();
//	SetThreadLocale( locale );
//	LCID lcidRussianDefault = MAKESORTLCID( MAKELANGID( LANG_RUSSIAN, SUBLANG_DEFAULT ), SORT_DEFAULT, 0 );
//	LCID lcidRussianNeutral = MAKESORTLCID( MAKELANGID( LANG_RUSSIAN, SUBLANG_NEUTRAL ), SORT_DEFAULT, 0 );
//	SetThreadLocale( lcidRussianDefault );
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////

LPVOID LoadResource( LPCTSTR id, LPCTSTR type, DWORD* lpSize = NULL )
{
	UINT_PTR uID = (UINT_PTR)(PVOID)id;
	uID;

#ifdef TASKMANEX_EXE
	HMODULE hMod = GetModuleHandle( NULL );
#else
	HMODULE hMod = AfxGetResourceHandle(); // theApp.m_hInstance
#endif
//	LANGID Lang = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
//	LANGID Lang = GetUserDefaultLangID(); // User defined
	LANGID Lang = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ); // Windows internal???
//	LANGID Lang = MAKELANGID( LANG_RUSSIAN, SUBLANG_DEFAULT );
	HRSRC hRsrc = NULL;

	hRsrc = FindResourceEx( hMod, type, id, Lang );

	if( hRsrc == NULL )
	{
		TRACE( _T("FindResourceEx: try #2 (%Id)\n"), uID );
		hRsrc = FindResourceEx( hMod, type, id, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL) );
	}

	if( hRsrc == NULL )
	{
		TRACE( _T("FindResource: try #3 (%Id)\n"), uID );
		hRsrc = FindResource( hMod, id, type );
	}

/*
	int typeX;
	for( typeX=0; hRsrc == NULL && typeX<25; typeX++ )
	{
		TRACE( _T("FindResource: retry type = #%d\n"), typeX );
		hRsrc = FindResource( hMod, id, MAKEINTRESOURCE(typeX) );
	}
	if( hRsrc == NULL ) 
	{
		for( typeX=0; hRsrc == NULL && typeX<25; typeX++ )
		{
			for( int i=0; i<65536; i++ )
			{
				hRsrc = FindResource( hMod, MAKEINTRESOURCE(i), MAKEINTRESOURCE(typeX) );
				if( hRsrc != NULL )
				{
					TRACE( _T("FindResource: found type #%d, id = %d\n"), typeX, i );
				}
			}
		}
	}
//*/
	if( hRsrc == NULL ) 
		return NULL;

	if( lpSize != NULL )
	{
		*lpSize = SizeofResource( hMod, hRsrc );
	}

	HGLOBAL hGlob = LoadResource( hMod, hRsrc );
	if( hGlob == NULL ) 
		return NULL;
	LPVOID pRsrc = LockResource( hGlob );
	return pRsrc;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

BOOL	LocLoadString		( UINT id, LPTSTR pString, IN OUT LPDWORD lpdwSize )
{
	UINT uID = id;

	UINT idBlock = (uID & 0xFFFF)/16 +1;
	UINT idOffset = uID % 16;

	LPVOID pRsrc = LoadResource( MAKEINTRESOURCE(idBlock), RT_STRING, NULL );
	if( pRsrc == NULL ) 
	{
		*lpdwSize = 0;
		return FALSE;
	}

	LPWSTR str = (LPWSTR) pRsrc;
	for( UINT i = 0; i < idOffset; i++)
	{
		str += *str + 1;
	}

	DWORD dwSize = (*str);
	if( dwSize >= *lpdwSize )
	{
		*lpdwSize = dwSize + 1;
		return FALSE;
	}

	*lpdwSize = dwSize + 1;
	int nChars = dwSize;

#ifdef _UNICODE
	memcpy( pString, str+1, nChars * sizeof(WCHAR) );
#else
	WideCharToMultiByte( CP_ACP, 0, str+1, nChars, pString, nChars, NULL, NULL );
#endif

	pString[nChars] = L'\0';
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TASKMANEX_EXE
////////////////////////////////////////////////////////////////////////////////////////////////

HMENU	LocLoadMenu			( LPCTSTR id )
{
	LPVOID pRsrc = LoadResource( id, RT_MENU );
	if( pRsrc == NULL ) 
		return NULL;
	HMENU hRsrc = LoadMenuIndirect( pRsrc );
	return hRsrc;
}

CString	LocLoadString		( LPCTSTR id )
{
	UINT uID = (UINT)(PVOID)id;

	//ASSERT( IS_INTRESOURCE(id) );
	ASSERT( (uID&0xFFFF) == uID );

	UINT idBlock = (uID & 0xFFFF)/16 +1;
	UINT idOffset = uID % 16;

	LPVOID pRsrc = LoadResource( MAKEINTRESOURCE(idBlock), RT_STRING, NULL );
	if( pRsrc == NULL ) 
		return _T("");

	LPWSTR str = (LPWSTR) pRsrc;
	for( UINT i = 0; i < idOffset; i++)
	{
		str += *str + 1;
	}

	DWORD dwSize = (*str);

	int nChars = dwSize;
	WCHAR* p = new WCHAR[ nChars + 1 ];
	if( p == NULL )
		return _T("");

	memcpy( p, str+1, nChars * sizeof(WCHAR) );
	p[nChars] = L'\0';

	CString s = p;
	delete[] p;

	return s;
}

HICON	LocLoadIcon			( LPCTSTR id, int cx, int cy )
{
	DWORD dwSize;
	LPVOID pRsrc;
	LPCTSTR id2 = 0;
	
	dwSize = 0;
	pRsrc = LoadResource( id, RT_GROUP_ICON, &dwSize ); //RT_ICON );
	if( pRsrc != NULL ) 
	{
		LPGRPICONDIR pDir = (LPGRPICONDIR) pRsrc;
		int idIcon = LookupIconIdFromDirectoryEx( (PBYTE)pDir, TRUE, cx, cy, LR_DEFAULTCOLOR );
		id2 = MAKEINTRESOURCE(idIcon);
	}
	else
	{
		id2 = id;
	}

	dwSize = 0;
	pRsrc = LoadResource( id2, RT_ICON, &dwSize );
	if( pRsrc == NULL ) 
		return NULL;

//	LPICONIMAGE lpImage = (LPICONIMAGE)pRsrc;

	HICON hRsrc = CreateIconFromResource( (PBYTE)pRsrc, dwSize, TRUE, 0x00030000 );
	return hRsrc;
}

struct ACCELTABLEENTRY { 
	WORD fFlags; 
	WORD wAnsi; 
	WORD wId; 
	WORD padding; 
};

HACCEL  LocLoadAccelerators	( LPCTSTR id )
{
//	UINT uID = (UINT)(PVOID)id;
//	TRACE( _T("LocLoadAccelerators: (%d)\n"), uID );
//	HACCEL hRsrc = LoadAccelerators( AfxGetResourceHandle(), id );
//	return hRsrc;
	DWORD dwSize = 0;
	LPVOID pRsrc = LoadResource( id, RT_ACCELERATOR, &dwSize );
	if( pRsrc == NULL ) 
		return NULL;
	ACCELTABLEENTRY* pSrc = (ACCELTABLEENTRY*)pRsrc;
	int n = dwSize/sizeof(ACCELTABLEENTRY);
	ACCEL* pDest = new ACCEL[n];
	if( pDest == NULL )
		return NULL;

	for( int i=0; i<n; i++ )
	{
		pDest[i].fVirt = (BYTE)pSrc[i].fFlags;
		pDest[i].key = pSrc[i].wAnsi;
		pDest[i].cmd = pSrc[i].wId;
	}

	HACCEL hRsrc = CreateAcceleratorTable( pDest, n );

	delete[] pDest;
	return hRsrc;
}

LPCDLGTEMPLATE LocLoadDialog( LPCTSTR id )
{
	DWORD dwSize = 0;
	LPVOID pRsrc = LoadResource( id, RT_DIALOG, &dwSize );
	if( pRsrc == NULL ) 
		return NULL;
	return (LPCDLGTEMPLATE) pRsrc;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

HMENU	LocLoadMenu			( UINT id )
{
	HMENU hRsrc = LocLoadMenu( MAKEINTRESOURCE(id) );
	return hRsrc;
}

CString	LocLoadString		( UINT id )
{
	CString s = LocLoadString( MAKEINTRESOURCE(id) );
	return s;
}

HICON	LocLoadIcon			( UINT id, int cx, int cy )
{
	HICON hRsrc = LocLoadIcon( MAKEINTRESOURCE(id), cx, cy );
	return hRsrc;
}

HACCEL  LocLoadAccelerators	( UINT id )
{
	HACCEL hRsrc = LocLoadAccelerators( MAKEINTRESOURCE(id) );
	return hRsrc;
}

LPCDLGTEMPLATE LocLoadDialog( UINT id )
{
	LPCDLGTEMPLATE hRsrc = LocLoadDialog( MAKEINTRESOURCE(id) );
	return hRsrc;
}

////////////////////////////////////////////////////////////////////////////////////////////////
#endif //ifndef TASKMANEX_EXE
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
