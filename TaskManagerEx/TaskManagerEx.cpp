// TaskManagerEx.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "LoadDll.h"
#include <locale.h>
#include "SplashWnd.h"
#include "Resource.h"

#include "../TaskManagerExDll/Localization.h"
#include "../TaskManagerExDll/TaskManagerEx.h"
#include "../TaskManagerExDll/Taskmgr.h"
#include "../TaskManagerExDll/Options.h"
#include "../TaskManagerExDll/TaskManagerExDllExport.h"

//#define ENUM_ALL_WINDOWS

DWORD	dwWindowsNTMajorVersion = 0;

HANDLE hAnotherTaskManagerExEvent = NULL;
LPCTSTR szGlobalKernelName = _T("Local\\TaskManagerExIsAlreadyRunning");

static char g_szStamp_Mark[] = "\r\n\r\nTaskManagerEx.cpp Timestamp: " __DATE__ ", " __TIME__ "\r\n\r\n";

//////////////////////////////////////////////////////////////////////////

BOOL IsAnotherTaskManagerEx_Capture( BOOL bInitialState )
{
	if( hAnotherTaskManagerExEvent != NULL )
	{
		return TRUE;
	}
	hAnotherTaskManagerExEvent = CreateEvent( NULL, TRUE, bInitialState, szGlobalKernelName );	

	DWORD dwer = GetLastError();	

	return (dwer==ERROR_ALREADY_EXISTS);
}

//////////////////////////////////////////////////////////////////////////

void ReleaseAnotherTaskManagerEx()
{
	if( hAnotherTaskManagerExEvent != NULL )
	{
		CloseHandle( hAnotherTaskManagerExEvent );
		hAnotherTaskManagerExEvent = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////

DWORD GetTaskManagerProcessID( HWND hwnd )
{
	DWORD pID = 0;

	GetWindowThreadProcessId( hwnd, &pID );

	return pID;
}

//////////////////////////////////////////////////////////////////////////

//void MySleep( DWORD dwMilliseconds ) // for simplier debug
//{
//	Sleep(dwMilliseconds);
//}

DWORD MyWaitForSingleObject( HANDLE hHandle, DWORD dwMilliseconds ) // for simplier debug
{
	DWORD res = WaitForSingleObject( hHandle, dwMilliseconds );
	return res;
}

//////////////////////////////////////////////////////////////////////////

TCHAR szDllPath[_MAX_PATH] = _T(""); // TaskManagerEx.dll name

//////////////////////////////////////////////////////////////////////////

BOOL IsHookedTaskManagerWindow( HWND hwnd )
{
	#ifdef _DEBUG
//	OutputDebugString( _T("TaskManagerEx: GetWindowLong( hwnd, GWL_USERDATA );\n") );
	#endif

	LONG lUserData = GetWindowLong( hwnd, GWL_USERDATA );
	if( lUserData == TASKMANAGEREX_WINDOW_LONG_USER_MAGIC_VALUE )
	{
		return TRUE;
	}

	#ifdef _DEBUG
	OutputDebugString( _T("TaskManagerEx: SendMessage( hwnd, WM_TASKMANAGER_FLAG );\n") );
	#endif

	LRESULT lRes = SendMessage( hwnd, WM_TASKMANAGER_FLAG, 0, 0 );
	if( lRes == WM_TASKMANAGER_FLAG_RET_VAL )
	{
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

BOOL LoadDllToRemoteTaskManager( HWND hwnd )
{
	// Check if window has already been hooked:
	BOOL bIsHooked = IsHookedTaskManagerWindow( hwnd );
	if( bIsHooked )
	{
		return FALSE;
	}

	DWORD processId = GetTaskManagerProcessID( hwnd );
	#ifdef _DEBUG
	TCHAR s[200] = _T("");
	wsprintf( s, _T("TaskManagerEx> LoadDllForRemoteThread for PID = %d\n"),
		processId );
	OutputDebugString( s );
	#endif

	using namespace RemoteExecute;
	DWORD dwRet = LoadDllForRemoteThread( processId, 
			LOAD_LIBRARY, DONT_FREE_LIBRARY, NO_SPECIAL_MODE,
			szDllPath, "Initialize",
			NoFuncReturn, NoFuncReturnSpecial, NoLastError,
			DONT_RETURN_REMOTE_API_ERRORS,
			ZeroArguments, NoArguments );

	BOOL bOK = dwRet == 0;

	if( !bOK )
	{
		#ifdef _DEBUG
		TCHAR s[200] = _T("");
		wsprintf( s, _T("TaskManagerEx> LoadDllForRemoteThread for PID = %d returned %d (0x%X)\n"),
			processId, dwRet, dwRet );
		OutputDebugString( s );
		#endif
	}

	BOOL bRunOnceOnly = FALSE;

	{
		HKEY hKey = NULL;
		DWORD dwType = REG_DWORD;
		DWORD dwSize = sizeof(DWORD);
		LONG lRes;
		DWORD dwValue;

		::RegOpenKey( REG_ROOT, REG_KEY, &hKey );

		if( hKey != NULL )
		{
			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			lRes = ::RegQueryValueEx( hKey, _T("LoaderStartOnceOnly"), NULL, &dwType, 
					(LPBYTE)&dwValue, &dwSize );

			if( lRes == ERROR_SUCCESS )
			{
				bRunOnceOnly = dwValue != 0;
			}

			::RegCloseKey( hKey );
		}
	}

	if( bRunOnceOnly )
	{
		ResetEvent( hAnotherTaskManagerExEvent ); // Stop process after one Dll load
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

#ifdef ENUM_ALL_WINDOWS

BOOL CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam )
{
	lParam;

	{
		LPCTSTR pszOriginalCaption = GetTaskManagerCaption();
		TCHAR szCaption[ 200 ] = _T("");
		GetWindowText( hwnd, szCaption, SIZEOF_ARRAY(szCaption) );

		if( 0 != lstrcmp( szCaption, pszOriginalCaption ) )
		{
			goto exit;
		}

		LoadDllToRemoteTaskManager( hwnd );
	}

exit:
	return TRUE;
}

#endif //ifdef ENUM_ALL_WINDOWS

//////////////////////////////////////////////////////////////////////////

VOID CALLBACK ThreadTimerProc(
	HWND hwnd,
    UINT uMsg,
    UINT idEvent,
    DWORD dwTime
)
{
	hwnd;
	uMsg;
	idEvent;
	dwTime;

	DWORD res = MyWaitForSingleObject( hAnotherTaskManagerExEvent, 0 );
	if( res != WAIT_OBJECT_0 )
	{
		PostQuitMessage( 0 );
	}

#ifdef ENUM_ALL_WINDOWS
	EnumWindows( EnumWindowsProc, 0 );
#else
	HWND hTaskMgr = FindTaskManagerWindow();
	if( hTaskMgr != NULL )
	{
		LoadDllToRemoteTaskManager( hTaskMgr );
	}
#endif
}

//////////////////////////////////////////////////////////////////////////

//LPCTSTR g_szLocale = NULL;

//////////////////////////////////////////////////////////////////////////

int MainFunction()
{
//	g_szLocale = _tsetlocale( LC_ALL, _T("") ); // it is commented because CRT was reduced from this project
												// so there is no need in localization of CRT library
	BOOL res;
	DWORD dwBufSize;

	TCHAR szCaption[200] = _T("");
	TCHAR szRequireNT[200] = _T("");
	TCHAR szGetModuleFilenameError[200] = _T("");
	TCHAR szCommandLineHelp[5000] = _T("");

	dwBufSize = SIZEOF_ARRAY(szCaption);
	LocLoadString(IDS_MESSAGEBOX_CAPTION, szCaption, &dwBufSize );
	dwBufSize = SIZEOF_ARRAY(szRequireNT);
	LocLoadString(IDS_REQUIRE_NT, szRequireNT, &dwBufSize );
	dwBufSize = SIZEOF_ARRAY(szGetModuleFilenameError);
	LocLoadString(IDS_GETMODULEFILNAME_ERROR, szGetModuleFilenameError, &dwBufSize );
	dwBufSize = SIZEOF_ARRAY(szCommandLineHelp);
	LocLoadString(IDS_COMMANDLINE_HELP, szCommandLineHelp, &dwBufSize );

	LPWSTR szCommandLineW = GetCommandLineW();
	if( szCommandLineW == NULL )
	{
		return -1;
	}
	int argc = 0;
	LPWSTR* argv = CommandLineToArgvW( szCommandLineW, &argc );
	if( argv == NULL )
	{
		MessageBox( NULL, szRequireNT, szCaption, MB_OK | MB_ICONERROR );
		return -1;
	}

	BOOL bAnother	= FALSE;

	BOOL bUnload	= FALSE;
	BOOL bRestart	= FALSE;
	BOOL bUninstall	= FALSE;

	if( argc > 1 )
	{
		bUnload		= 0 == lstrcmpiW( argv[1], L"/u" );
		bRestart	= 0 == lstrcmpiW( argv[1], L"/r" );
		bUninstall	= 0 == lstrcmpiW( argv[1], L"/clean" );

		if( bUninstall )
		{
			// Remove TaskManagerEx from startup:
			COptions options;
			options.Uninstall();

			// Unload TaskManagerEx.exe process:

			bAnother = IsAnotherTaskManagerEx_Capture( TRUE );
			if( bAnother )
			{
				ResetEvent( hAnotherTaskManagerExEvent );
			}
			ReleaseAnotherTaskManagerEx();

			// Restart existing (hooked) Taskmgr.exe:

			HWND hTaskMgr = FindTaskManagerWindow();
			BOOL bIsHooked = IsHookedTaskManagerWindow( hTaskMgr );
			if( bIsHooked )
			{
				//DWORD dwThreadId = GetWindowThreadProcessId( hTaskMgr, NULL );
				SendMessage( hTaskMgr, WM_CLOSE, 0, 0 );
				//PostThreadMessage( dwThreadId, WM_QUIT, 0, 0 );

				TCHAR szCommand[] = _T("taskmgr.exe");

				STARTUPINFO si;
				PROCESS_INFORMATION pi;
				ZeroMemory( &si, sizeof(si) );
				si.cb = sizeof(si);
				ZeroMemory( &pi, sizeof(pi) );
				BOOL res = CreateProcess( NULL, szCommand,
					NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi );
				if( res )
				{
					CloseHandle( pi.hProcess );
					CloseHandle( pi.hThread );
				}
			}

			GlobalFree( argv );
			return 0;
		}
		else if( bUnload )
		{
			bAnother = IsAnotherTaskManagerEx_Capture( TRUE );
			if( bAnother )
			{
				ResetEvent( hAnotherTaskManagerExEvent );
			}
			ReleaseAnotherTaskManagerEx();
			GlobalFree( argv );
			return 0;
		}
		else if( bRestart )
		{
			bAnother = IsAnotherTaskManagerEx_Capture( TRUE );
			if( bAnother )
			{
				ResetEvent( hAnotherTaskManagerExEvent );
				DWORD dwRet = MyWaitForSingleObject( hAnotherTaskManagerExEvent, 3000 );
				if( dwRet != WAIT_OBJECT_0 )
				{
					ASSERT( FALSE && "WaitForSingleObject error!" );
					GlobalFree( argv );
					return -1;
				}
			}
			bAnother = FALSE;
		}
		else
		{
			MessageBox( NULL, szCommandLineHelp, szCaption, MB_OK | MB_ICONINFORMATION );
			GlobalFree( argv );
			return 1;
		}
	}
	else
	{
		bAnother = IsAnotherTaskManagerEx_Capture( TRUE );
	}

	GlobalFree( argv );
	argv = NULL;

	if( bAnother != FALSE )
	{
		return 0;
	}

	if( hAnotherTaskManagerExEvent == NULL )
	{
		ASSERT(FALSE);
		return -1;
	}

	res = GetModuleFileName( NULL, szDllPath, _MAX_PATH );
	if ( !res )
	{
		MessageBox( NULL, szGetModuleFilenameError, szCaption, MB_OK | MB_ICONERROR );
		return -2;
	}

	LPTSTR p = NULL;

	for( int i = lstrlen(szDllPath)-1; i>=0; i-- )
	{
		if( szDllPath[i] == _T('\\') )
		{
			p = &szDllPath[i];
			break;
		}
	}

	if( p == NULL )
	{
	   p = szDllPath;
	}
	else
	{
	   p++;
	}

	lstrcpy( p, TMEX_INJECT_DLL );

	//////////////////////////////////////////////////////////////////////////
	//Let's work

	COptions options;
	BOOL bSplash = options.GetShowSplash();

	if( bSplash )
	{
		res = CreateSplashScreen( 1000 );
	}

	SetTimer( NULL, 0, 400, ThreadTimerProc );

	MSG msg;

	while( (res = GetMessage( &msg, NULL, 0, 0 )) != 0)
	{ 
		if (res == -1)
		{
			// handle the error and possibly exit
			break;
		}
		else
		{
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}
	} 

	SetEvent( hAnotherTaskManagerExEvent );
	ReleaseAnotherTaskManagerEx();

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////
///  CRT:

/*
#ifdef SUBSYSTEM_WINDOWS

int CALLBACK _tWinMain( HINSTANCE, HINSTANCE, LPTSTR, int )
{
	int res = MainFunction();
	return res;
}

#else

int _tmain( int, TCHAR ** ) //( int argc, TCHAR **argv )
{
	int res = MainFunction();
	return res;
}

#endif
//*/

//////////////////////////////////////////////////////////////////////////
