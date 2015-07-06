// TaskManagerExDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "TaskManagerExDll.h"
#include "HookImportFunction.h"
#include "ApiHooks.h"
#include "LoadDll.h"
#include "SystemInfo.h"
#include "SystemInfoDlg.h"
#include "AboutExtension.h"
#include "InformationDlg.h"

#include "TaskManagerEx.h"
#include "Localization.h"
#include "Taskmgr.h"
#include <locale.h>
#include <winuser.h>

#include "TipDlg.h"
#include "OptionsPropertySheet.h"

#include "../AccessMaster/SecurityInformation.h"

#define CREATE_FAKE_WINDOW
//#define VISIBLE_FAKE_WINDOW

//#ifndef MIIM_STRING
//#define MIIM_STRING      0x00000040		// Windows 2000+
//#endif

//#pragma data_seg(".SharedData")
volatile BOOL bInitialized = FALSE;
//#pragma data_seg()
//#pragma comment( linker, "/SECTION:.SharedData,RWS" )

#pragma comment( linker, "/BASE:0x5BD00000" )
//#pragma comment( linker, "/OUT:TaskManagerEx.dll" )

static char g_szStamp_Mark[] = "\r\n\r\nTaskManagerExDll.cpp Timestamp: " __DATE__ ", " __TIME__ "\r\n\r\n";

////////////////////////////////////////////////////////////////////////////////////////////////////

#define DEFAULT_PID_WIDTH	50
#define TASKMAN_CORRECTION	4	// -4 because Taskmgr.exe (in Windows XP) has some problem
								// in calculation and horizontal scrollbar appears too early

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MyShellExecute( HWND hwnd, LPCTSTR szOperation, LPCTSTR szFileName )
{
	if( _tcscmp( szFileName, LocLoadString(IDS_PID_SYSTEM_NAME) ) == 0 )
	{
		AfxMessageBox( LocLoadString(IDS_SYSTEM_RPOCESS_OPERATION_DENIED) );
		return FALSE;
	}

	SHELLEXECUTEINFO sei;
	ZeroMemory( &sei, sizeof(sei) );
	sei.cbSize = sizeof(sei);
	sei.fMask = SEE_MASK_FLAG_DDEWAIT | SEE_MASK_INVOKEIDLIST;
	sei.hwnd = hwnd;
	sei.lpVerb = szOperation;
	sei.lpFile = szFileName;
	sei.nShow = SW_SHOWDEFAULT;
	BOOL res = ShellExecuteEx( &sei );
	if( !res )
	{
		CString s;
		s.Format( LocLoadString(IDS_CANT_SHELL_EXECUTE),
			szOperation, szFileName );
		AfxMessageBox( s );
	}
	return res;
}

BOOL IsDependencyWalkerInstalled()
{
	BOOL bDependency = FALSE;
	HKEY hKey = NULL;
	long lRes = RegOpenKeyEx( HKEY_CLASSES_ROOT, _T("CLSID\\{A324EA60-2156-11D0-826F-00A0C9044E61}"),
		0, KEY_READ, &hKey );

	if( lRes == ERROR_SUCCESS )
	{
		bDependency = TRUE;
		RegCloseKey( hKey );
		hKey = NULL;
	}
	return bDependency;
}

BOOL FreeSearchInString( LPCTSTR lpszString, LPCTSTR lpszSubString )
{
	if ( lpszSubString[0] == _T('\0') )
		return TRUE;

	CString szString( lpszString );
	CString szSubString( lpszSubString );

	szString.MakeUpper();
	szSubString.MakeUpper();

	return _tcsstr( szString, szSubString ) != NULL;
}

BOOL GetProcessExecutableName( DWORD dwPID, LPTSTR szName, int cbSize )
{
	if( szName == NULL )
		return FALSE;

	if( cbSize == 0 )
		return FALSE;

	szName[0] = _T('\0');

	if( !IPsapi::bStatus )
		return FALSE;

	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID );
	if( hProcess == NULL )
	{
		if( IS_PID_SYSTEM(dwPID) ) // [System]
		{
			_tcsncpy( szName, LocLoadString(IDS_PID_SYSTEM_NAME), cbSize );
			return TRUE;
		}
		return FALSE;
	}

	DWORD dwRet = IPsapi::GetModuleFileNameEx( hProcess, NULL, szName, cbSize );
	szName[dwRet] = _T('\0');
	CloseHandle(hProcess);

	if( dwRet == 0 )
	{
		if( IS_PID_SYSTEM(dwPID) ) // [System]
		{
			_tcsncpy( szName, LocLoadString(IDS_PID_SYSTEM_NAME), cbSize );
			return TRUE;
		}
	}

	if( dwRet > 0 )
	{
		CString strFileName = SystemInfoUtils::DecodeModuleName( szName );
		_tcsncpy( szName, strFileName, cbSize );
	}

	return dwRet > 0;
}

//////////////////////////////////////////////////////////////

CString GetProcessName( DWORD dwPID )
{
	TCHAR szFileName[MAX_PATH] = _T("");
	GetProcessExecutableName( dwPID, szFileName, SIZEOF_ARRAY(szFileName) );
	LPTSTR p = _tcsrchr( szFileName, _T('\\') );
	if( p == NULL )
	{
		p = szFileName;
	}
	else
	{
		p++;
	}
	return CString(p);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

TaskManagerExDllApp theApp;

TaskManagerExDllApp::TaskManagerExDllApp(): CWinApp( TMEX_APP_NAME )
{
	mapProcessIsService.InitHashTable( 1982 );
	m_hwndFakeWindow = NULL;
	fnOriginFakeWndProc = NULL;
}

TaskManagerExDllApp::~TaskManagerExDllApp()
{
	MicroImageList.DeleteImageList();
}

LPCTSTR g_szLocale = NULL;

BOOL TaskManagerExDllApp::InitInstance()
{
	g_szLocale = _tsetlocale( LC_ALL, _T("") );
	SetThreadNativeLanguage();

	SetRegistryKey( TMEX_REGISTRY_KEY );

	BOOL bEnable = AfxEnableMemoryTracking(FALSE);
	if( m_pszProfileName != NULL )
	{
		free( (void*)m_pszProfileName );
		m_pszProfileName = NULL;
	}
	m_pszProfileName = _tcsdup(TMEX_PROFILE_NAME);
	AfxEnableMemoryTracking(bEnable);

	if ( bInitialized )
		return FALSE;

	//hMainIconSmall = hMainIconBig = LocLoadIcon( IDI_SYSTEM_INFO_ICON );

	hMainIconSmall = LocLoadIcon( IDI_SYSTEM_INFO_ICON, 16, 16 );
	hMainIconBig = LocLoadIcon( IDI_SYSTEM_INFO_ICON, 32, 32 );

	EnableDebugPriv();

	return TRUE;
}

int TaskManagerExDllApp::ExitInstance()
{  
	if ( fnOriginFakeWndProc != NULL )
		SetWindowLong(
				m_hwndFakeWindow,
				GWL_WNDPROC,
				(LONG)fnOriginFakeWndProc );

	if( m_hwndFakeWindow != NULL )
	{
		DestroyWindow( m_hwndFakeWindow );
		m_hwndFakeWindow = NULL;
	}

	if ( bInitialized )
	{
		Deinitialize();

		_exit(0);
	}

	return CWinApp::ExitInstance();
}

DWORD TaskManagerExDllApp::GetProcessesListDlgID()
{
	return TASKMGR_PROCESS_LIST_DLG_ID;
}

DWORD TaskManagerExDllApp::GetApplicationsListDlgID()
{
	return TASKMGR_APPLICATIONS_LIST_DLG_ID;
}

BOOL TaskManagerExDllApp::IsAnotherTaskManagerExDll()
{
   CString strMutexName;
   strMutexName.Format( _T("Local\\TaskManagerExDllIsAlreadyLoaded_PID_%08X"), GetCurrentProcessId() );

   HANDLE hMutex = CreateMutex( NULL, FALSE, strMutexName );	
//   HANDLE hMutex = CreateMutex( NULL, FALSE, _T("Local\\TaskManagerExDllIsAlreadyLoaded") );	
   
   DWORD dwer = GetLastError();	

   return (hMutex==NULL || dwer==ERROR_ALREADY_EXISTS);
}

BOOL TaskManagerExDllApp::Initialize()
{
	if ( IsAnotherTaskManagerExDll() )
	  return FALSE;

	hwndTaskManager = FindTaskManagerWindow();
	hwndProcessesList = FindTaskManagerDlgItem( GetProcessesListDlgID() );
	hwndApplicationsList = FindTaskManagerDlgItem( GetApplicationsListDlgID() );

	DWORD TaskManagerPID = 0;
	GetWindowThreadProcessId( hwndTaskManager, &TaskManagerPID );
	if( TaskManagerPID != GetCurrentProcessId() )
	{
		return FALSE;
	}
	
	if ( hwndTaskManager == NULL || 
		  hwndProcessesList == NULL ||
		  hwndApplicationsList == NULL )
		return FALSE;

	fnOriginTaskManagerWndProc = (WNDPROC)SetWindowLong( 
			hwndTaskManager,
			GWL_WNDPROC,
			(LONG)TaskManagerWndProc );

	fnOriginProcessesList = (WNDPROC)SetWindowLong( 
			hwndProcessesList,
			GWL_WNDPROC,
			(LONG)ProcessesListWndProc );

	fnOriginProcessesTab = (WNDPROC)SetWindowLong( 
			GetParent(hwndProcessesList),
			GWL_WNDPROC,
			(LONG)ProcessesTabWndProc );

	fnOriginApplicationsList = (WNDPROC)SetWindowLong( 
			hwndApplicationsList,
			GWL_WNDPROC,
			(LONG)ApplicationsListWndProc );

	fnOriginApplicationsTab = (WNDPROC)SetWindowLong( 
			GetParent(hwndApplicationsList),
			GWL_WNDPROC,
			(LONG)ApplicationsTabWndProc );

	if ( fnOriginProcessesList == NULL || 
		 fnOriginTaskManagerWndProc == NULL ||
		 fnOriginProcessesTab == NULL ||
		 fnOriginApplicationsList == NULL ||
		 fnOriginApplicationsTab == NULL )
	{
		Deinitialize();
		return FALSE;
	}

	HookImportedFunctions();

	SetProcessesIcons( m_options.GetShowProcessIcons() );

	UpdateMainMenu( GetMenu(hwndTaskManager) );

	SendMessage( hwndTaskManager, WM_NCPAINT, (WPARAM)1, (LPARAM)0 );

	SendMessage( hwndTaskManager, WM_TASKMANAGER_CREATE_FAKIE_WND, 0, 0 );

	SendMessage( hwndTaskManager, WM_SETTEXT, 0, 0 );	// To change window caption to new one!

	UpdateApplicationsListView();

	RefreshProcessList();

	bInitialized = TRUE;

	PostMessage( hwndTaskManager, WM_TASKMANAGER_TIP_OF_THE_DAY, 0, 0 ); // to show tip of the day at startup

	return TRUE;
}

BOOL TaskManagerExDllApp::Deinitialize()
{
	if ( fnOriginTaskManagerWndProc != NULL )
		SetWindowLong( 
				hwndTaskManager,	
				GWL_WNDPROC, 
				(LONG)fnOriginTaskManagerWndProc );

	if ( fnOriginProcessesList != NULL )
		SetWindowLong( 
				hwndProcessesList,	
				GWL_WNDPROC, 
				(LONG)fnOriginProcessesList );

	if ( fnOriginProcessesTab != NULL )
		SetWindowLong( 
				GetParent(hwndProcessesList),	
				GWL_WNDPROC, 
				(LONG)fnOriginProcessesTab );

	if ( fnOriginApplicationsList != NULL )
		SetWindowLong( 
				hwndApplicationsList,	
				GWL_WNDPROC, 
				(LONG)fnOriginApplicationsList );

	if ( fnOriginApplicationsTab != NULL )
		SetWindowLong( 
				GetParent(hwndApplicationsList),	
				GWL_WNDPROC, 
				(LONG)fnOriginApplicationsTab );

	return TRUE;
}

LPCTSTR TaskManagerExDllApp::GetNewTaskManagerCaption()
{
	static LPCTSTR lpszNewCaption = NULL;
	static TCHAR szNewCaption[0x100];

	if ( lpszNewCaption == NULL )
	{
		_tcscpy( szNewCaption, GetTaskManagerCaption() );
		_tcscat( szNewCaption, LocLoadString(IDS_CAPTION_EXTENDED) );
		lpszNewCaption = szNewCaption;
	}

	return lpszNewCaption;
}

/*
BOOL TaskManagerExDllApp::InitFakeWindow(HINSTANCE hinstance) 
{ 
    WNDCLASSEX wcx; 
 
    // Fill in the window class structure with parameters 
    // that describe the main window. 
 
    wcx.cbSize = sizeof(wcx);          // size of structure 
    wcx.style = CS_HREDRAW | 
        CS_VREDRAW;                    // redraw if size changes 
    wcx.lpfnWndProc = TaskManagerExDllApp::FakeWndProc;  // points to window procedure 
    wcx.cbClsExtra = 0;                // no extra class memory 
    wcx.cbWndExtra = 0;                // no extra window memory 
    wcx.hInstance = hinstance;         // handle to instance 
    wcx.hIcon = NULL;                  // predefined app. icon 
    wcx.hCursor = NULL;                // predefined arrow 
    wcx.hbrBackground = NULL;          // white background brush 
    wcx.lpszMenuName =  NULL;          // name of menu resource 
    wcx.lpszClassName = TASKMGR_DEFAULT_WINDOW_CLASS;  // name of window class 
    wcx.hIconSm = NULL; 
 
    // Register the window class. 
 
    return RegisterClassEx(&wcx); 
} 
*/


LRESULT CALLBACK TaskManagerExDllApp::FakeWndProc(
  HWND hwnd,      // handle to window
  UINT uMsg,      // message identifier
  WPARAM wParam,  // first message parameter
  LPARAM lParam   // second message parameter
)
{
	switch( uMsg )
	{
	case WM_TASKMANAGER_FLAG:
		//TRACE( _T("TaskManagerExDllApp::FakeWndProc: WM_TASKMANAGER_FLAG\n") );
		return WM_TASKMANAGER_FLAG_RET_VAL;
		break;

	}

	switch( uMsg )
	{
	case WM_CLOSE:
	case WM_TASKMAN_SET_FOREGROUND:
		TRACE( _T("TaskManagerExDllApp::FakeWndProc: WM_TASKMAN_SET_FOREGROUND\n") );
		// Some internal Task Manager message
		// It is used to activate TaskManager Window.
		//TRACE( _T("TaskManagerExDllApp::FakeWndProc() got message WM_TASKMAN_SET_FOREGROUND!\n") );

		{
			LRESULT result = CallWindowProc( 
						theApp.fnOriginTaskManagerWndProc, 
						theApp.hwndTaskManager, 
						uMsg, 
						wParam, 
						lParam );

			return result;
		}

		break;
	}

//	switch( uMsg )
//	{
//	case WM_ACTIVATE:
//	case WM_SHOWWINDOW:
//
//		{
//			LRESULT result = CallWindowProc( 
//						theApp.fnOriginTaskManagerWndProc, 
//						theApp.hwndTaskManager, 
//						uMsg, 
//						wParam, 
//						lParam );
//
//#ifndef VISIBLE_FAKE_WINDOW
//			return result;
//#else
//			result;
//#endif
//		}

//		break;
//	}


//	LRESULT result = DefWindowProc(
//			hwnd, 
//			uMsg, 
//			wParam, 
//			lParam );

	LRESULT result = CallWindowProc( 
				theApp.fnOriginFakeWndProc,
				hwnd, 
				uMsg, 
				wParam, 
				lParam );

	return result;
}

BOOL TaskManagerExDllApp::CreateFakeWindow()
{
#ifndef CREATE_FAKE_WINDOW
	return TRUE;
#endif

	HINSTANCE hinstance = AfxGetInstanceHandle();
//	BOOL res = InitFakeWindow( hinstance );
//	TRACE( _T("TaskManagerExDllApp::InitFakeWindow() returned %d\n"), res );
//	if( !res )
//	{
//		//return FALSE;
//	}

    HWND hwnd; 
 
	LPCTSTR szDefaultCaption = GetTaskManagerCaption();
	HWND hParent = NULL;

    hwnd = CreateWindow( 
        TASKMGR_DEFAULT_WINDOW_CLASS, // name of window class 
        szDefaultCaption,    // title-bar string 
        WS_OVERLAPPEDWINDOW, // top-level window 
        CW_USEDEFAULT,       // default horizontal position 
        CW_USEDEFAULT,       // default vertical position 
        CW_USEDEFAULT,       // default width 
        CW_USEDEFAULT,       // default height 
        hParent,             // no owner window 
        (HMENU) NULL,        // use class menu 
        hinstance,           // handle to application instance 
        (LPVOID) NULL);      // no window-creation data 
 
    if (!hwnd) 
        return FALSE; 
 
	fnOriginFakeWndProc = (WNDPROC)SetWindowLong( 
			hwnd,
			GWL_WNDPROC,
			(LONG)FakeWndProc );

	SetWindowLong( hwnd, GWL_USERDATA, TASKMANAGEREX_WINDOW_LONG_USER_MAGIC_VALUE );

    // Show the window and send a WM_PAINT message to the window 
    // procedure. 

#ifdef VISIBLE_FAKE_WINDOW
	int nCmdShow = SW_SHOW;
#else
	int nCmdShow = SW_HIDE;
#endif

	ShowWindow(hwnd, nCmdShow); 
	UpdateWindow(hwnd);
	return TRUE;
}

BOOL CALLBACK UpdateWndZorderProc2( HWND hwnd, LPARAM lParam )
{
	BOOL bTopLevel = (BOOL)lParam;
	//if( ::GetParent( hwnd ) == NULL ) // Top-level windows:
	{
		DWORD dwWindowProcessId = 0;
		GetWindowThreadProcessId( hwnd, &dwWindowProcessId );
		if( dwWindowProcessId == GetCurrentProcessId() )
		{
			if( hwnd != theApp.hwndTaskManager )
			{
				if( bTopLevel )
				{
					::SetWindowPos( hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
				}
				else
				{
					::SetWindowPos( hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
				}
				//TCHAR szCaption[200] = _T("");
				//::GetWindowText( hwnd, szCaption, SIZEOF_ARRAY(szCaption) );
				//TRACE( _T("UpdateWndZorderProc2(): window 0x%08X (%s), bTopLevel = %d\n"),
				//	hwnd, szCaption, bTopLevel );
			}
		}
	}
	return TRUE;
}

bool TaskManagerExDllApp::IsTaskManagerWindowTopMost()
{
	bool res = false;
	HMENU hMenu = GetMenu( theApp.hwndTaskManager );
	if( hMenu == NULL )
		return res;
	int cnt = GetMenuItemCount( hMenu );
	if( cnt == -1 )
		return res;

	for( int i=0; i<cnt; i++ )
	{
		HMENU hSubMenu = GetSubMenu( hMenu, i );
		if( hSubMenu == NULL )
			continue;

		MENUITEMINFO info;
		info.cbSize = sizeof(info);
		info.fMask = MIIM_STATE;
		BOOL bFound = GetMenuItemInfo( hSubMenu, TASKMGR_ALWAYS_ON_TOP_CMD, FALSE, &info );
		if( bFound )
		{
			bool bChecked = 0 != ( info.fState & MFS_CHECKED );
			//TRACE( _T("TaskManagerExDllApp::IsTaskManagerWindowTopMost(): Found \"Always On Top\" menu element!\n") );
			return bChecked;
			//break;
		}
	}

	return res;
}


LRESULT CALLBACK TaskManagerExDllApp::TaskManagerWndProc(
  HWND hwnd,      // handle to window
  UINT uMsg,      // message identifier
  WPARAM wParam,  // first message parameter
  LPARAM lParam   // second message parameter
)
{
	switch( uMsg )
	{
	case WM_TASKMAN_SET_FOREGROUND:
		TRACE( _T("TaskManagerExDllApp::TaskManagerWndProc: WM_TASKMAN_SET_FOREGROUND\n") );
		break;

	case WM_TASKMANAGER_FLAG: // this case is not necessary now, because we are changing window caption
		//TRACE( _T("TaskManagerExDllApp::TaskManagerWndProc: WM_TASKMANAGER_FLAG\n") );
		return WM_TASKMANAGER_FLAG_RET_VAL;
		break;

	case WM_TASKMANAGER_TIP_OF_THE_DAY:
		theApp.ShowTipAtStartup();
		break;

	case WM_TASKMANAGER_CREATE_FAKIE_WND:
		theApp.CreateFakeWindow();
		break;
/*
	case WM_KEYDOWN:
		TRACE( _T("TaskManagerExDllApp - WM_KEYDOWN\n") );
		break;

	case WM_KEYUP:
		TRACE( _T("TaskManagerExDllApp - WM_KEYUP\n") );
		break;
*/
	case WM_COMMAND:
		switch( wParam )
		{
			case ID_EXTENSION_CPU00:
				theApp.m_options.SetAlertLimit(0);
				theApp.RefreshProcessList();
				break;

			case ID_EXTENSION_CPU25:
				theApp.m_options.SetAlertLimit(25);
				theApp.RefreshProcessList();
				break;

			case ID_EXTENSION_CPU50:
				theApp.m_options.SetAlertLimit(50);
				theApp.RefreshProcessList();
				break;

			case ID_EXTENSION_CPU75:
				theApp.m_options.SetAlertLimit(75);
				theApp.RefreshProcessList();
				break;

			case ID_EXTENSION_FINDUSEDFILE:
			case ID_EXTENSION_FINDUSEDMODULE:
				CSystemInfoDlgThread::Start( 0, wParam, TRUE, TRUE );
				break;

			case ID_EXTENSION_ALL_HANDLES:
				CSystemInfoDlgThread::Start( 0, wParam, FALSE, TRUE );
				break;

			case ID_EXTENSION_SHOW_DRIVERS:
				CSystemInfoDlgThread::Start( 0, wParam, FALSE, TRUE );
				break;

			case ID_EXTENSION_PROCESSICONS:
				theApp.m_options.SetShowProcessIcons( !theApp.m_options.GetShowProcessIcons() );
				theApp.SetProcessesIcons( theApp.m_options.GetShowProcessIcons() );
				break;

			case ID_EXTENSION_HIDESERVICES:
				theApp.m_options.SetHideServices( !theApp.m_options.GetHideServices() );
				theApp.RefreshProcessList();
				break;

			case ID_EXTENSION_TIP_OF_THE_DAY:
				{
					CTipDlg dlg( CWnd::FromHandle(hwnd) );
					dlg.DoModal();
				}
				break;

			case ID_EXTENSION_ABOUT:
				CAboutExtensionThread::Start( CWnd::FromHandle(hwnd) );
				break;

			case ID_EXTENSION_OPTIONS:
				{
					COptionsPropertySheet dlg( CWnd::FromHandle(hwnd) );
					dlg.DoModal();
				}
				break;
			case TASKMGR_ALWAYS_ON_TOP_CMD:
				//TRACE( _T("TaskManagerEx: TASKMGR_ALWAYS_ON_TOP_CMD\n") );
				{
					bool bOLD_AlwaysOnTop = theApp.IsTaskManagerWindowTopMost();
					//TRACE( _T("TaskManagerEx: TASKMGR_ALWAYS_ON_TOP_CMD: bOLD_AlwaysOnTop = %d\n"), bOLD_AlwaysOnTop );
					EnumWindows( UpdateWndZorderProc2, (LPARAM)!bOLD_AlwaysOnTop );
				}
				break;

		}
		break;

	case WM_INITMENUPOPUP:
		{
		  HMENU hMenu = (HMENU)wParam;
		  switch ( GetMenuItemID( hMenu, 0 ) )
		  {
		  case TMEX_EXTENSION_MENU_FIRST_ITEM:
			 CheckMenuItem( hMenu, ID_EXTENSION_HIDESERVICES, MF_BYCOMMAND | 
					  ( theApp.m_options.GetHideServices() ? MF_CHECKED : MF_UNCHECKED ) );
			 CheckMenuItem( hMenu, ID_EXTENSION_PROCESSICONS, MF_BYCOMMAND | 
					  ( theApp.m_options.GetShowProcessIcons() ? MF_CHECKED : MF_UNCHECKED ) );
			 break;

		 case TMEX_CPU_MENU_FIRST_ITEM:
			 for ( int i = 0; i < 4; i++ )
				CheckMenuItem( hMenu, i, MF_BYPOSITION | MF_UNCHECKED );

			 CheckMenuItem( hMenu, theApp.m_options.GetAlertLimit()/25, MF_BYPOSITION | MF_CHECKED );
			 break;
		  }
		}

		break;

	case WM_SETTEXT:
		{
			LPCTSTR szCaption = theApp.GetNewTaskManagerCaption();
			lParam = (LPARAM)szCaption;
		}
		break;
	}

	LRESULT result = CallWindowProc( 
				theApp.fnOriginTaskManagerWndProc, 
				hwnd, 
				uMsg, 
				wParam, 
				lParam );

	return result;
}

LRESULT CALLBACK TaskManagerExDllApp::ProcessesListWndProc(
  HWND hwnd,      // handle to window
  UINT uMsg,      // message identifier
  WPARAM wParam,  // first message parameter
  LPARAM lParam   // second message parameter
)
{
	switch( uMsg )
	{
	case LVM_SETITEMA:
	case LVM_SETITEMW:
	case LVM_INSERTITEMA:
	case LVM_INSERTITEMW:
		{
			LVITEM* pItem = (LVITEM*)lParam;

			if ( !( pItem->mask & LVIF_IMAGE ) && pItem->iSubItem == 0 )
			{
				if( theApp.m_options.GetShowProcessIcons() )
				{
					pItem->mask |= LVIF_IMAGE;
					pItem->iImage = I_IMAGECALLBACK;
				}
			}
		}
		break;
/*
	case WM_KEYDOWN:
		TRACE( _T("ProcessesListWndProc - WM_KEYDOWN\n") );
		break;

	case WM_KEYUP:
		TRACE( _T("ProcessesListWndProc - WM_KEYUP\n") );
//		break;
*/

//	case WM_KEYDOWN:	// don't work for some reason
	case WM_KEYUP:
		switch( wParam )
		{
		case VK_DELETE:
			{
				ProcessesItemData* pData = theApp.GetSelectedProcessData();
				if ( pData != NULL )
				theApp.KillProcess( pData->processId );
			}
			break;

/* don't work for some reason
		case VK_RETURN:
			{
				TRACE( _T("ProcessesListWndProc - VK_RETURN\n") );
				SHORT keyAlt = GetKeyState( VK_MENU );
				BOOL bAlt = (keyAlt & 0x8000) != 0;
				TRACE( _T("Alt = %X, %d - VK_RETURN\n"), keyAlt, bAlt );
				if( bAlt )
				{
					ProcessesItemData* pData = theApp.GetSelectedProcessData();
					// Open process to read to query the module list
					if( pData != NULL )
					{
						TCHAR szFileName[MAX_PATH] = _T("");
						BOOL res = GetProcessExecutableName( pData->processId, szFileName, SIZEOF_ARRAY(szFileName) );

						if( res )
						{
							MyShellExecute( hwnd, SHELL_OPERATION_FILE_PROPERTIES, szFileName );
						}
						else
						{
						}
					}
				}
			}
			break;
*/
		}
		break;

	case LVM_SETIMAGELIST:
		{
			//int iImageCount = ImageList_GetImageCount( (HIMAGELIST)lParam );
			//TRACE( _T("ProcessesListWndProc> LVM_SETIMAGELIST> count = %d\n"), iImageCount );
		}
		break;

	case LVM_GETIMAGELIST:
		{
			//int iImageCount = ImageList_GetImageCount( (HIMAGELIST)lParam );
			//TRACE( _T("ProcessesListWndProc> LVM_GETIMAGELIST> count = %d\n"), iImageCount );
		}
		break;
   }

	LRESULT rc = CallWindowProc( 
			theApp.fnOriginProcessesList, 
			hwnd, 
			uMsg, 
			wParam, 
			lParam );

	return rc;
}

LRESULT CALLBACK TaskManagerExDllApp::ProcessesTabWndProc(
  HWND hwnd,      // handle to window
  UINT uMsg,      // message identifier
  WPARAM wParam,  // first message parameter
  LPARAM lParam   // second message parameter
)
{
   LRESULT rc;
   
	switch( uMsg )
	{
/*
	case NM_KEYDOWN:
		TRACE( _T("ProcessesTabWndProc - NM_KEYDOWN\n") );
		break;

	case NM_RETURN:
		TRACE( _T("ProcessesTabWndProc - NM_RETURN\n") );
		break;

	case WM_KEYDOWN:
		TRACE( _T("ProcessesTabWndProc - WM_KEYDOWN\n") );
		break;

	case WM_KEYUP:
		TRACE( _T("ProcessesTabWndProc - WM_KEYUP\n") );
		break;
*/
	case WM_COMMAND:
		switch( wParam )
		{
		case ID_PROCESSES_FILES:
		case ID_PROCESSES_MODULES:
		case ID_PROCESSES_HANDLES:
		case ID_PROCESSES_WINDOWS:
		case ID_PROCESSES_THREADS:
		case ID_PROCESSES_MEMORY:
			{
				ProcessesItemData* pData = theApp.GetSelectedProcessData();
				if ( pData != NULL )
				{
					if( wParam == ID_PROCESSES_MODULES && IS_PID_SYSTEM(pData->processId) )
					{
						CSystemInfoDlgThread::Start( pData->processId, ID_EXTENSION_SHOW_DRIVERS, FALSE, TRUE );
					}
					else
					{
						CSystemInfoDlgThread::Start( pData->processId, wParam, FALSE, FALSE );
					}
				}
			}
			break;

		case ID_PROCESSES_INFO:
			{
				ProcessesItemData* pData = theApp.GetSelectedProcessData();
				if ( pData != NULL )
				{
					CInformationDlgThread::Start( pData->processId, wParam );
				}
			}
			break;

		case ID_PROCESSES_PROCESS_SECURITY:
			{
				ProcessesItemData* pData = theApp.GetSelectedProcessData();
				// Open process to read to query the module list
				if( pData != NULL )
				{
					CString strProcessName = GetProcessName( pData->processId );
					HANDLE hProcess = OpenProcess( MAXIMUM_ALLOWED, FALSE, pData->processId );
					if( hProcess != NULL )
					{
						ObjectInformation info;
						info.m_hHandle = hProcess;
						info.m_szName[0] = _T('\0');
						_tcsncpy( info.m_szObjectName, strProcessName, SIZEOF_ARRAY(info.m_szObjectName) );
						info.m_objInternalType = OB_TYPE_PROCESS;

						CSecurityInformation* pSec = CSecurityInformation::CreateInstance( info, FALSE );
						if( pSec != NULL )
						{
							pSec->EditSecurity( hwnd );
							pSec->Release();
						}

						CloseHandle( hProcess );
					}
				}
			}
			break;

		case ID_PROCESSES_EXE_DEPENDENCY:
			{
				ProcessesItemData* pData = theApp.GetSelectedProcessData();
				// Open process to read to query the module list
				if( pData != NULL )
				{
					TCHAR szFileName[MAX_PATH] = _T("");
					BOOL res = GetProcessExecutableName( pData->processId, szFileName, SIZEOF_ARRAY(szFileName) );

					if( res )
					{
						MyShellExecute( hwnd, SHELL_OPERATION_VIEW_DEPENDENCY, szFileName );
					}
					else
					{
					}
				}
			}
			break;

		case ID_PROCESSES_EXE_PROPERTIES:
			{
				ProcessesItemData* pData = theApp.GetSelectedProcessData();
				// Open process to read to query the module list
				if( pData != NULL )
				{
					TCHAR szFileName[MAX_PATH] = _T("");
					BOOL res = GetProcessExecutableName( pData->processId, szFileName, SIZEOF_ARRAY(szFileName) );

					if( res )
					{
						MyShellExecute( hwnd, SHELL_OPERATION_FILE_PROPERTIES, szFileName );
					}
					else
					{
					}
				}
			}
			break;
	   }
	   break;

   case WM_NOTIFY:
      {
         LPNMLISTVIEW  pnm    = (LPNMLISTVIEW)lParam;

         if ( pnm->hdr.hwndFrom == theApp.hwndProcessesList )
            switch ( pnm->hdr.code )
            {
            case NM_CUSTOMDRAW:
      	      {
                  LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)lParam;

                  switch(lplvcd->nmcd.dwDrawStage) 
                  {
                  case CDDS_PREPAINT :
                     return CDRF_NOTIFYITEMDRAW;

                  case CDDS_ITEMPREPAINT:
                     {
                        ProcessesItemData* pData = theApp.GetProcessData( lplvcd->nmcd.dwItemSpec );
						BOOL bService = theApp.IsProcessService( pData->processId );

                        RECT iconRect;
                        if ( ListView_GetItemRect( theApp.hwndProcessesList, lplvcd->nmcd.dwItemSpec, &iconRect, LVIR_ICON ) )
                        {
							HBRUSH hWindowBrush = GetSysColorBrush(COLOR_WINDOW);
							FillRect( lplvcd->nmcd.hdc, &iconRect, hWindowBrush );

							if( theApp.m_options.GetShowProcessIcons() )
							{
								const int cx = 16;
								const int cy = 16;
								int iImage = theApp.FindProcessImageIndex( pData->processId );
								if( iImage == I_IMAGECALLBACK ) // image list index not found
								{
									static HICON hServiceIcon =
										LocLoadIcon( MAKEINTRESOURCE(IDI_SERVICEPROCESS), cx, cy );
									static HICON hDefaultIcon =
										LocLoadIcon( MAKEINTRESOURCE(IDI_DEFAULTPROCESS), cx, cy );
									HICON hIcon = hDefaultIcon;
									if( bService )
									{
										hIcon = hServiceIcon;
									}
									DrawIconEx( lplvcd->nmcd.hdc, iconRect.left, iconRect.top, hIcon,
										cx, cy, 0, hWindowBrush, DI_NORMAL );
								}
							}
                        }
						else
						{
							TRACE( _T("ListView_GetItemRect error\n") );
						}

                        if ( pData == NULL )
	                        return CDRF_NEWFONT;

						int limit = theApp.m_options.GetAlertLimit();

                        if ( limit != 0 &&
                              pData->usageCPU > limit )
                           lplvcd->clrText = theApp.m_options.GetAlertColor();
                        else 
                        if ( theApp.m_options.GetHideServices() && bService )
                           lplvcd->clrText = theApp.m_options.GetServiceColor();
                        else
                           lplvcd->clrText = theApp.m_options.GetProcessColor();

                        return CDRF_NEWFONT;
                     }
                     break;
                  }
               }
               break;
            }
      }
      break;

	case WM_INITMENUPOPUP:
		if ( GetMenuItemID( (HMENU)wParam, 0 ) == TASKMGR_END_PROCESS_CMD )
		{
			ProcessesItemData* pData = theApp.GetSelectedProcessData();
			if ( pData != NULL && pData->processId != 0 )
			{
				BOOL bDependency = IsDependencyWalkerInstalled();
				HMENU hMenu = (HMENU)wParam;

				HMENU hSubMenu = LocLoadMenu( MAKEINTRESOURCE(IDR_PROCESS_POPUP) );
				HMENU hSubMenu2 = GetSubMenu( hSubMenu, 0 );

				int iItemCount = GetMenuItemCount( hSubMenu2 );
				int pos = 0;
				for( int i=0; i<iItemCount; i++, pos++ )
				{
					int iID = GetMenuItemID( hSubMenu2, i );

					if( iID == ID_TASKMGR_DEFAULT_ITEMS )
					{
						pos = GetMenuItemCount( hSubMenu2 );
						continue;
					}

					if( iID == ID_PROCESSES_EXE_DEPENDENCY && !bDependency )
					{
						continue;
					}

					MENUITEMINFO mii;
					ZeroMemory( &mii, sizeof(mii) );
					TCHAR szItemText[200] = _T("");
					mii.cbSize = sizeof(mii);
					mii.fType = MFT_STRING;
					mii.fMask = MIIM_TYPE; // MIIM_STRING; // MIIM_STRING - Windows 2000+
					mii.dwTypeData = szItemText;
					mii.cch = SIZEOF_ARRAY(szItemText);
					GetMenuItemInfo( hSubMenu2, i, TRUE, &mii);

					DWORD dwState = GetMenuState( hSubMenu2, i, MF_BYPOSITION );

					InsertMenu( hMenu, pos, MF_BYPOSITION | dwState, iID, szItemText );
				}
				DestroyMenu( hSubMenu );
			}
		}
		break;
	}

	rc = CallWindowProc( 
				theApp.fnOriginProcessesTab, 
				hwnd, 
				uMsg, 
				wParam, 
				lParam );

	switch( uMsg )
	{
	case WM_NOTIFY:
      {
         LPNMLISTVIEW  pnm    = (LPNMLISTVIEW)lParam;

         if ( pnm->hdr.hwndFrom == theApp.hwndProcessesList )
            switch ( pnm->hdr.code )
            {
            case LVN_GETDISPINFOW:
               {
                  NMLVDISPINFO* pDispInfo = (NMLVDISPINFO*)lParam;

                  ProcessesItemData* pData = theApp.GetProcessData( pDispInfo->item.iItem );
/*
				  {
					  TRACE(
						  //_T("TaskManagerEx> LVN_GETDISPINFOW: ")
						  _T("PID: %d |")
						  _T(", %5d")
						  _T(", %5d |")
						  _T("\n"),
						  pData->processId,
						  pData->usageCPU,
						  pData->Handles,
						  pData->Threads,
						  0
						  );
				  }
*/

                  if ( ( pDispInfo->item.mask & LVIF_IMAGE ) &&
                       ( pDispInfo->item.iSubItem == 0 ) )
                     pDispInfo->item.iImage = theApp.FindProcessImageIndex( pData->processId );
               }
               break;
            }
      }
      break;
	}

   return rc;  
}

LRESULT CALLBACK TaskManagerExDllApp::ApplicationsListWndProc(
  HWND hwnd,      // handle to window
  UINT uMsg,      // message identifier
  WPARAM wParam,  // first message parameter
  LPARAM lParam   // second message parameter
)
{
	switch( uMsg )
	{
	case LVM_SETCOLUMNWIDTH:
		if( wParam == 0 )
		{
			short width = LOWORD(lParam);
			if( width == LVSCW_AUTOSIZE )
			{
			}
			else if( width == LVSCW_AUTOSIZE_USEHEADER )
			{
			}
			else if( width<0 )
			{
			}
			else
			{
				width = (short) max( 0, width - DEFAULT_PID_WIDTH - TASKMAN_CORRECTION );
				lParam = MAKELPARAM( width, 0 );
			}
		}
		break;

	case LVM_SETIMAGELIST:
		{
			//int iImageCount = ImageList_GetImageCount( (HIMAGELIST)lParam );
			//TRACE( _T("ApplicationsListWndProc> LVM_SETIMAGELIST> count = %d\n"), iImageCount );
		}
		break;

	case LVM_GETIMAGELIST:
		{
			//int iImageCount = ImageList_GetImageCount( (HIMAGELIST)lParam );
			//TRACE( _T("ApplicationsListWndProc> LVM_GETIMAGELIST> count = %d\n"), iImageCount );
		}
		break;
	}

	LRESULT rc = CallWindowProc( 
			theApp.fnOriginApplicationsList, 
			hwnd, 
			uMsg, 
			wParam, 
			lParam );

	return rc;
}

LRESULT CALLBACK TaskManagerExDllApp::ApplicationsTabWndProc(
  HWND hwnd,      // handle to window
  UINT uMsg,      // message identifier
  WPARAM wParam,  // first message parameter
  LPARAM lParam   // second message parameter
)
{
	LRESULT rc = CallWindowProc( 
					theApp.fnOriginApplicationsTab, 
					hwnd, 
					uMsg, 
					wParam, 
					lParam );

	switch( uMsg )
	{
	case WM_NOTIFY:
		{
			LPNMLISTVIEW  pnm    = (LPNMLISTVIEW)lParam;

			if ( pnm->hdr.hwndFrom == theApp.hwndApplicationsList )
				switch ( pnm->hdr.code )
				{
				case LVN_GETDISPINFOW:
					{
						NMLVDISPINFO* pDispInfo = (NMLVDISPINFO*)lParam;

						if ( pDispInfo->item.mask & LVIF_TEXT )
							switch ( pDispInfo->item.iSubItem )
							{
							case APPLICATION_LIST_PID_COLUMN:
								{
									ApplicationsItemData* pData =
										theApp.GetApplicationsData( pDispInfo->item.iItem );
									DWORD pID;
									GetWindowThreadProcessId( pData->hWnd, &pID );
									wsprintfW( (wchar_t*)pDispInfo->item.pszText, L"%d", pID );
								}
								break;
							}
					}
					break;
				}
		}
		break;
	}

	return rc;
}

TASKMANAGEREXDLL_API BOOL WINAPI Initialize()
{
	TRACE( _T("TaskManagerEx.dll> Initialize() enter\n") );
	BOOL res = theApp.Initialize();
	TRACE( _T("TaskManagerEx.dll> Initialize() returned %d\n"), res );

	return res;
}

void TaskManagerExDllApp::UpdateMainMenu( HMENU hMenu )
{
//	TRACE( _T("Try to UpdateMainMenu...\n") );
	if ( GetMenuItemID( GetSubMenu( hMenu, TMEX_EXTENSION_SUBMENU_INDEX ), 0 ) == TMEX_EXTENSION_MENU_FIRST_ITEM )
		return;

	SetThreadNativeLanguage();

	HMENU hSubMenu = LocLoadMenu( MAKEINTRESOURCE(IDR_EXTENSION) );
	HMENU hSubMenu2 = GetSubMenu( hSubMenu, 0 );
	ASSERT( GetMenuItemID( hSubMenu2, 0 ) == TMEX_EXTENSION_MENU_FIRST_ITEM );

	MENUITEMINFO mii;
	ZeroMemory( &mii, sizeof(mii) );
	TCHAR szItemText[200] = _T("");
	mii.cbSize = sizeof(mii);
	mii.fType = MFT_STRING;
	mii.fMask = MIIM_TYPE; // MIIM_STRING; // MIIM_STRING - Windows 2000+
	mii.dwTypeData = szItemText;
	mii.cch = SIZEOF_ARRAY(szItemText);
	GetMenuItemInfo( hSubMenu, 0, TRUE, &mii);

	::InsertMenu( hMenu, TMEX_EXTENSION_SUBMENU_INDEX, MF_BYPOSITION | MF_POPUP, (int)hSubMenu2, szItemText );
}

void TaskManagerExDllApp::SetProcessesIcons( BOOL bShow )
{
	if( bShow )
	{
		ListView_SetImageList( hwndProcessesList,
				ListView_GetImageList(hwndApplicationsList, LVSIL_SMALL ),
				LVSIL_SMALL  );
	}
	else
	{
		HIMAGELIST hOldList = ListView_SetImageList( hwndProcessesList, NULL, LVSIL_SMALL );
		if( hOldList != NULL )
		{
			// there was another image list, so we can't simply remove old list.
			// If we set image list to NULL, then first column's width will remain unchanged.
			// But if we set image list with images 1x1, then it will seem that there is no icon at all!
			MicroImageList.DeleteImageList();
			MicroImageList.Create( 1, 1, ILC_COLOR, 1, 1 );
			ListView_SetImageList( hwndProcessesList, MicroImageList, LVSIL_SMALL );
		}
	}

	int processCnt = ListView_GetItemCount( hwndProcessesList );
	for ( int nItem = 0; nItem < processCnt; nItem++ )
	{
		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.iItem = nItem;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_IMAGE;
		lvi.iImage = bShow ? I_IMAGECALLBACK : 0;
		::SendMessage( hwndProcessesList, LVM_SETITEM, 0, (LPARAM)&lvi );
	}

	RefreshProcessList();
}

void TaskManagerExDllApp::RefreshProcessList()
{
	InvalidateRect( hwndProcessesList, NULL, TRUE );
}

void TaskManagerExDllApp::UpdateApplicationsListView()
{
	LVCOLUMN lvc;
	::ZeroMemory( &lvc, sizeof(lvc) );

	lvc.mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;
	lvc.pszText = _T("PID");
	lvc.cx = DEFAULT_PID_WIDTH;
	lvc.fmt = HDF_RIGHT;
	ListView_InsertColumn( hwndApplicationsList, APPLICATION_LIST_PID_COLUMN, &lvc );

	ListView_SetExtendedListViewStyle( hwndApplicationsList, 
         ListView_GetExtendedListViewStyle( hwndApplicationsList ) |
         LVS_EX_FULLROWSELECT ) ;

	int iStatus	= ListView_GetColumnWidth( hwndApplicationsList, 1 );
	RECT rectClient = {0,0,0,0};
	GetClientRect( hwndApplicationsList, &rectClient );

	ListView_SetColumnWidth( hwndApplicationsList, 0, rectClient.right - rectClient.left - iStatus + TASKMAN_CORRECTION );

	UpdateWindow( hwndApplicationsList );
}


TaskManagerExDllApp::ProcessesItemData* TaskManagerExDllApp::GetSelectedProcessData()
{
   return GetProcessData( ListView_GetNextItem( hwndProcessesList, -1, LVNI_SELECTED ) );
}

TaskManagerExDllApp::ProcessesItemData* TaskManagerExDllApp::GetProcessData( int nItem )
{
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.iItem = nItem;
	lvi.mask = LVIF_PARAM;

	::SendMessage( hwndProcessesList, LVM_GETITEM, 0, (LPARAM)&lvi );

	return (ProcessesItemData*)lvi.lParam;
}

TaskManagerExDllApp::ApplicationsItemData* TaskManagerExDllApp::GetApplicationsData( int nItem )
{
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.iItem = nItem;
	lvi.mask = LVIF_PARAM;

	::SendMessage( hwndApplicationsList, LVM_GETITEM, 0, (LPARAM)&lvi );

	ApplicationsItemData* pItemData = (ApplicationsItemData*)lvi.lParam;

	//TRACE( _T("AppItemData: '%ws', %d\n"),
	//	pItemData->Caption, pItemData->index );

	return pItemData;
}

BOOL CALLBACK TaskManagerExDllApp::_EnumChildProc( HWND hwnd, LPARAM lParam )
{
	StructFindTaskManagerDlgItem* pParam = (StructFindTaskManagerDlgItem*)lParam;

	if ( (DWORD)GetDlgCtrlID( hwnd ) == pParam->itemID )
	{
		pParam->hwnd = hwnd;

		return FALSE;
	}

	return TRUE;
}

HWND TaskManagerExDllApp::FindTaskManagerDlgItem( DWORD itemID )
{
	StructFindTaskManagerDlgItem param;

	param.itemID = (DWORD)itemID;
	param.hwnd = NULL;

	EnumChildWindows(  hwndTaskManager, _EnumChildProc, (LPARAM)&param );

	return param.hwnd;
}

BOOL TaskManagerExDllApp::IsProcessIdValid( DWORD pID )
{
	HANDLE hDup = ::OpenProcess( PROCESS_DUP_HANDLE, TRUE, pID );

	if ( hDup != NULL )
		CloseHandle( hDup );

	return hDup != NULL;
}

DWORD TaskManagerExDllApp::_IsService( DWORD pID, BOOL& isService )
{
	HANDLE hProcessToken = NULL;
	DWORD groupLength = 40;
	PTOKEN_GROUPS groupInfo = NULL;

	SID_IDENTIFIER_AUTHORITY siaNt = SECURITY_NT_AUTHORITY;
	PSID pInteractiveSid = NULL;
	PSID pServiceSid = NULL;

	DWORD dwRet = NO_ERROR;
    
    // reset flags
	BOOL isInteractive = FALSE;
	isService = FALSE;

	DWORD ndx;

	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, pID );

	// open the token
	if (!::OpenProcessToken( hProcess, TOKEN_QUERY, &hProcessToken) )
	{
		dwRet = ::GetLastError();
		goto closedown;
	}

	// allocate a buffer of default size
	groupInfo = (PTOKEN_GROUPS)::LocalAlloc( LMEM_FIXED, groupLength );
	if (groupInfo == NULL)
	{
		dwRet = ::GetLastError();
		goto closedown;
	}

	// try to get the info
	ZeroMemory( groupInfo, groupLength );
	if (!::GetTokenInformation(hProcessToken, TokenGroups,
		groupInfo, groupLength, &groupLength))
	{
		// if buffer was too small, allocate to proper size, otherwise error
		if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			dwRet = ::GetLastError();
			goto closedown;
		}

		::LocalFree(groupInfo);

		groupInfo = (PTOKEN_GROUPS)::LocalAlloc( LMEM_FIXED, groupLength );
		if (groupInfo == NULL)
		{
			dwRet = ::GetLastError();
			goto closedown;
		}

		ZeroMemory( groupInfo, groupLength );
		if (!GetTokenInformation(hProcessToken, TokenGroups,
			groupInfo, groupLength, &groupLength))
		{
			dwRet = ::GetLastError();
			goto closedown;
		}
	}

	// create comparison sids
	if (!AllocateAndInitializeSid(&siaNt, 1, SECURITY_INTERACTIVE_RID,
		0, 0, 0, 0, 0, 0, 0, &pInteractiveSid))
	{
		dwRet = ::GetLastError();
		goto closedown;
	}

	if (!AllocateAndInitializeSid(&siaNt, 1, SECURITY_SERVICE_RID,
		0, 0, 0, 0, 0, 0, 0, &pServiceSid))
	{
		dwRet = ::GetLastError();
		goto closedown;
	}

	// try to match sids
	for( ndx = 0; ndx < groupInfo->GroupCount; ndx ++ )
	{
		SID_AND_ATTRIBUTES sanda = groupInfo->Groups[ndx];
		PSID pSid = sanda.Sid;

		if (::EqualSid(pSid, pInteractiveSid))
		{
			isInteractive = TRUE;
			isService = FALSE;
			break;
		}
		else if (::EqualSid(pSid, pServiceSid))
		{
			isService = TRUE;
			isInteractive = FALSE;
			break;
		}
	}

   if ( !( isService || isInteractive ) )
		isService = TRUE;
        
	closedown:
		if ( pServiceSid )
			::FreeSid( pServiceSid );

		if ( pInteractiveSid )
			::FreeSid( pInteractiveSid );

		if ( groupInfo )
			::LocalFree( groupInfo );

		if ( hProcessToken )
			::CloseHandle( hProcessToken );

		if ( hProcess )
			::CloseHandle( hProcess );

	return dwRet;
}

BOOL TaskManagerExDllApp::IsProcessService( DWORD pID )
{
	BOOL ret = FALSE;

	if ( mapProcessIsService.Lookup( pID, ret ) )
		return ret;
	else
	{
		_IsService( pID, ret );
		mapProcessIsService.SetAt( pID, ret );
	}

	return ret;
}

int TaskManagerExDllApp::FindProcessImageIndex( DWORD pID )
{
	int processImageIndex = I_IMAGECALLBACK;

	int appsCnt = ListView_GetItemCount( hwndApplicationsList );

	ApplicationsItemData* pAppData = NULL;
	DWORD appPID = 0;

	for ( int i = 0; i < appsCnt; i++ )
	{
		pAppData = GetApplicationsData( i );

		GetWindowThreadProcessId( pAppData->hWnd, &appPID );

		if ( appPID == pID )
		{
			LVITEM lvi;
			ZeroMemory(&lvi, sizeof(lvi));
			lvi.iItem = i;
			lvi.mask = LVIF_IMAGE;
			::SendMessage( hwndApplicationsList, LVM_GETITEM, 0, (LPARAM)&lvi );

			processImageIndex = lvi.iImage;
			break;
		}
	}

	return processImageIndex;
}

void TaskManagerExDllApp::KillProcess( DWORD pId )
{
	HANDLE hProcess = OpenProcess( PROCESS_TERMINATE, 0, pId );

	if ( hProcess == NULL )
		return;

	TerminateProcess( hProcess, 0 );

	CloseHandle( hProcess );
}

void TaskManagerExDllApp::ShowTipAtStartup(void)
{
	// CG: This function added by 'Tip of the Day' component.

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if (cmdInfo.m_bShowSplash)
	{
		CTipDlg dlg( CWnd::FromHandle(hwndTaskManager) );
		if (dlg.m_bStartup)
			dlg.DoModal();
	}
}

void TaskManagerExDllApp::ShowTipOfTheDay(void)
{
	// CG: This function added by 'Tip of the Day' component.

	CTipDlg dlg;
	dlg.DoModal();
}

void TaskManagerExDllApp::OnProperties()
{
	// TODO: The property sheet attached to your project
	// via this function is not hooked up to any message
	// handler.  In order to actually use the property sheet,
	// you will need to associate this function with a control
	// in your project such as a menu item or tool bar button.

	COptionsPropertySheet propSheet;

	propSheet.DoModal();

	// This is where you would retrieve information from the property
	// sheet if propSheet.DoModal() returned IDOK.  We aren't doing
	// anything for simplicity.
}
