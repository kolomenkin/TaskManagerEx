// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__8659E6B5_4DF1_4F4C_BF70_FF0A41A38CA8__INCLUDED_)
#define AFX_STDAFX_H__8659E6B5_4DF1_4F4C_BF70_FF0A41A38CA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WINVER 0x0501
//#define WINVER 0x0500 // for multimonitor support

#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <tchar.h>
//#include <stdio.h>

//////////////////////////////////////////////////////////////////////////

#ifndef SIZEOF_ARRAY
#define SIZEOF_ARRAY(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

//////////////////////////////////////////////////////////////////////////

void MyTrace( LPCTSTR szFormat, ... );

//////////////////////////////////////////////////////////////////////////

#include <assert.h>
#define ASSERT assert

//////////////////////////////////////////////////////////////////////////

static bool g_bMoreLogging = false;

#define DO_TRACE_ALWAYS

#undef TRACE
#ifdef _DEBUG
#define TRACE	MyTrace
#else
#define TRACE	1 ? (void)0 : MyTrace
#endif

#ifdef DO_TRACE_ALWAYS
#undef TRACE
#define TRACE	MyTrace
#endif

//////////////////////////////////////////////////////////////////////////

//#include <atlbase.h>
//#define TRACE	ATLTRACE
//#define ASSERT	ATLASSERT

//////////////////////////////////////////////////////////////////////////

inline void my_memset(void* ptr, unsigned char ch, size_t bytes)
{
	unsigned char* p = (unsigned char*)ptr;
	for (size_t i = 0; i<bytes; i++)
	{
		*p = ch;
		p++;
	}
}

//////////////////////////////////////////////////////////////////////////

#define INT_TO_STR(x) #x
#define INT_TO_STR2(x) INT_TO_STR((x))
#define WARNING __FILE__  INT_TO_STR2(__LINE__) " : warning: "
//#pragma message (WARNING "Sample Text")

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__8659E6B5_4DF1_4F4C_BF70_FF0A41A38CA8__INCLUDED_)
