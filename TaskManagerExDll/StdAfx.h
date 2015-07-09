// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__8659E6B5_4DF1_4F4C_BF70_FF0A41A38CA8__INCLUDED_)
#define AFX_STDAFX_H__8659E6B5_4DF1_4F4C_BF70_FF0A41A38CA8__INCLUDED_

//#define WINVER	0x0500
#define WINVER	0x0501

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

//////////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxtempl.h>

#include <tchar.h>

#include <assert.h>
#undef ASSERT
#define ASSERT assert

#ifndef SIZEOF_ARRAY
#define SIZEOF_ARRAY(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

#define INT_TO_STR(x) #x
#define INT_TO_STR2(x) INT_TO_STR((x))
#define WARNING __FILE__  INT_TO_STR2(__LINE__) " : warning: "
//#pragma message (WARNING "Sample Text")
//#pragma message (WARNING "Hardcoded string!")

//////////////////////////////////////////////////////////////////////////

//#include "FlatToolTipCtrl.h"
typedef CToolTipCtrl CToolTipCtrlMine;
//typedef CToolTipCtrlEx CToolTipCtrlMine;

//#define TOOLTIP(id) m_tooltip.AddTool(GetDlgItem(id), id)
#define TOOLTIP(id) m_tooltip.AddTool( GetDlgItem(id), LocLoadString( id ) );

//////////////////////////////////////////////////////////////////////////

#include <stdio.h>

inline void MyTrace( LPCTSTR szFormat, ... )
{
	TCHAR szBuffer[1024] = _T("");
	va_list arglist;
	va_start(arglist, szFormat);   
	_vsntprintf( szBuffer, SIZEOF_ARRAY(szBuffer), szFormat, arglist);
	va_end(arglist);

	szBuffer[SIZEOF_ARRAY(szBuffer)-1] = _T('\0');
	OutputDebugString( szBuffer );
}

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

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__8659E6B5_4DF1_4F4C_BF70_FF0A41A38CA8__INCLUDED_)
