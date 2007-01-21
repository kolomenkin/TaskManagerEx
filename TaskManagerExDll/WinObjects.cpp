//////////////////////////////////////////////////////////////
// File:		// WinObjects.cpp
// File time:	// 26.04.2005	23:26
// Version: 	// 1.0
// Author:		// Sergey Kolomenkin <reinhard@tut.by>
// Copyright:	// (c) Sergey Kolomenkin, Minsk, 2005
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#include "stdafx.h"
//#include <windows.h>

#include "WinObjects.h"
#define WIN_OBJECT_IMPLEMENT
#include "WinObjects.h" // twice include!
#undef  WIN_OBJECT_IMPLEMENT

//////////////////////////////////////////////////////////////

BOOL DecodeWinObjectType( IN LPCTSTR typeToken, OUT OB_TYPE_ENUM& type )
{
	const DWORD count = (DWORD) OB_TYPES_COUNT;

	type = OB_TYPE_UNKNOWN;
	ASSERT( sizeof(g_ObjectTypes)/sizeof(g_ObjectTypes[0]) == OB_TYPES_COUNT );

	DWORD typeIndex = 0;
	for ( DWORD i = 0; i < count; i++ )
	{
		if ( lstrcmp( g_ObjectTypes[i].m_szName, typeToken ) == 0 )
		{
			typeIndex = i;
			type = g_ObjectTypes[i].m_objInternalType;
			break;
		}
	}

	if( type == OB_TYPE_UNKNOWN )
	{
		TRACE( _T("GetTypeFromTypeToken> Unknown type: \"%s\"!\n"), typeToken );
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////

BOOL GetWinObjectTypeInfo( IN OB_TYPE_ENUM& type, OUT ObjTypeInf& type_info )
{
#ifdef _DEBUG
	const DWORD count = (DWORD) OB_TYPES_COUNT;
#endif

	const DWORD index = (DWORD) type;
	ASSERT( index <count );
	type_info = g_ObjectTypes[index];
	ASSERT( type_info.m_objInternalType == type );
	return TRUE;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
