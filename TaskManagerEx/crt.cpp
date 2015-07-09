/// CRT simple implementation

#include "StdAfx.h"

#include <stdio.h>
//#pragma comment (lib, "ntdll.lib")

void MyTrace( LPCTSTR szFormat, ... )
{
	TCHAR szBuffer[1024] = _T("");
	va_list arglist;
	va_start(arglist, szFormat);
	_vsntprintf( szBuffer, SIZEOF_ARRAY(szBuffer), szFormat, arglist);
	va_end(arglist);

	szBuffer[SIZEOF_ARRAY(szBuffer)-1] = _T('\0');
	OutputDebugString( szBuffer );
}

/*
 /OPT:NOWIN98 /ENTRY:MyCRTStartup
*/
//extern "C" { int _afxForceEXCLUDE; }

//kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
#pragma  comment (lib, "kernel32.lib")
#pragma  comment (lib, "advapi32.lib")
#pragma  comment (lib, "user32.lib")
#pragma  comment (lib, "shell32.lib")

extern int MainFunction();

//int _tmain( int argc, TCHAR** argv )
int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int)
{
	hinstExe;
	pszCmdLine;
	TRACE(_T("TaskManagerEx> MainFunction begin\n"));
	int nRet = MainFunction();
	TRACE(_T("TaskManagerEx> MainFunction end\n"));
	return nRet;
}

extern "C" void MyCRTStartup()
{
	TRACE(_T("TaskManagerEx> MainFunction begin\n"));
	int nRet = MainFunction();
	TRACE(_T("TaskManagerEx> MainFunction end\n"));
	::ExitProcess((UINT)nRet);
}
