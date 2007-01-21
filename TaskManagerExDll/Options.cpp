//////////////////////////////////////////////////////////////
// File:		// Options.cpp
// File time:	// 16.04.2005	0:53
// Version: 	// 1.0
// Author:		// Sergey Kolomenkin <register@tut.by>
// Copyright:	// (c) Sergey Kolomenkin, Minsk, 2005
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#include "stdafx.h"
//#include <windows.h>
#include "Options.h"

//////////////////////////////////////////////////////////////

LPCTSTR COptions::m_szRegValues[VALUE_COUNT] = {0};

//////////////////////////////////////////////////////////////

#ifndef TASKMANEX_EXE

CString GetTaskManagerExe()
{
	TCHAR szModule[MAX_PATH] = _T("");
	GetModuleFileName( AfxGetInstanceHandle(), szModule, SIZEOF_ARRAY(szModule) );
	LPTSTR p = _tcsrchr( szModule, _T('\\') );
	if( p == NULL )
	{
		p = szModule;
	}
	else
	{
		p++;
	}
	_tcscpy( p, TMEX_LOADER_EXE );

	return CString(szModule);
}

#endif

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
