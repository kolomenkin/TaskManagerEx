// TaskManagerExDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "TaskManagerExDll.h"
#include "HookImportFunction.h"
#include "ApiHooks.h"
#include "LoadDll.h"
#include "SystemInfo.h"
#include "SystemInfoDlg.h"
#include "FindUsedDlg.h"
#include "AboutExtension.h"

#pragma data_seg(".SharedData")
BOOL bInitialized = FALSE;
#pragma data_seg()
#pragma comment( linker, "/SECTION:.SharedData,RWS" )

TaskManagerExDllApp theApp;

BOOL TaskManagerExDllApp::InitInstance()
{
   if ( bInitialized )
      return FALSE;

   dwWindowsNTMajorVersion = IsWindowsNT();

   if ( dwWindowsNTMajorVersion < 4 )
      return FALSE;

   hMainIcon = LoadIcon( IDI_MAINICON );

   EnableDebugPriv();

   LoadDefaultParameters();

   return TRUE;
}

TaskManagerExDllApp::ExitInstance()
{  
   SaveDefaultParameters();
   
   DestroyIcon( hMainIcon );
   
   if ( bInitialized )
   {
      Deinitialize();

      _exit(0);
   }

   return CWinApp::ExitInstance();
}

DWORD TaskManagerExDllApp::GetProcessesListDlgID()
{
	return 0x3F1;
}

DWORD TaskManagerExDllApp::GetApplicationsListDlgID()
{
	return 0x41D;
}

BOOL TaskManagerExDllApp::IsAnotherTaskManagerExDll()
{
   HANDLE hMutex = CreateMutex( NULL, FALSE, _T("TaskManagerExDllIsAlreadyLoaded") );	
   
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

   SetProcessesIcons( bShowProcessesIcons );
   
   UpdateMainMenu( GetMenu(hwndTaskManager) );

   SendMessage( hwndTaskManager, WM_NCPAINT, (WPARAM)1, (LPARAM)0 );

	SendMessage( hwndTaskManager, WM_SETTEXT, 0, 0 );

   UpdateApplicationsListView();
   
   RefreshProcessList();

   bInitialized = TRUE;

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

HWND TaskManagerExDllApp::FindTaskManagerWindow()
{
   return ::FindWindow( (LPCTSTR)32770, GetTaskManagerCaption() );
}

LPCTSTR TaskManagerExDllApp::GetEnglishTaskManagerCaption()
{
   LPCTSTR rc = NULL;

   switch ( dwWindowsNTMajorVersion )
   {
   case 4:
      rc = _T("Windows NT Task Manager");
      break;

   case 5:
   default:
      rc = _T("Windows Task Manager");
      break;
   }

   return rc;
}

// Thanks Dominique Faure for the idea
// dominique.faure@msg-software.com
LPCTSTR TaskManagerExDllApp::GetTaskManagerCaption()
{
   static LPCTSTR lpszCaption = NULL;
   static TCHAR szCaption[0x100];

   _tcscpy( szCaption, GetEnglishTaskManagerCaption() );

   if ( lpszCaption == NULL )
   {
      HINSTANCE hTaskmgrExe = LoadLibrary( _T("taskmgr.exe") );
      
      if ( hTaskmgrExe != NULL )
      {
         LoadString( hTaskmgrExe, 10003, szCaption, 0x100 );
         FreeLibrary( hTaskmgrExe );
      }

      lpszCaption = szCaption;
   }

   return lpszCaption;
}

LPCTSTR TaskManagerExDllApp::GetNewTaskManagerCaption()
{
   static LPCTSTR lpszNewCaption = NULL;
   static TCHAR szNewCaption[0x100];

   if ( lpszNewCaption == NULL )
   {
      _tcscpy( szNewCaption, GetTaskManagerCaption() );
      _tcscat( szNewCaption, _T(" (Extended)") );

      lpszNewCaption = szNewCaption;
   }

   return lpszNewCaption;
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
	case WM_COMMAND:
		switch( wParam )
		{
	   case ID_EXTENSION_CPU00:
	      theApp.iProcessUsageAlertLimit = 0;
	      theApp.RefreshProcessList();
	      break;

	   case ID_EXTENSION_CPU25:
	      theApp.iProcessUsageAlertLimit = 25;
	      theApp.RefreshProcessList();
	      break;

	   case ID_EXTENSION_CPU50:
	      theApp.iProcessUsageAlertLimit = 50;
	      theApp.RefreshProcessList();
	      break;

	   case ID_EXTENSION_CPU75:
	      theApp.iProcessUsageAlertLimit = 75;
	      theApp.RefreshProcessList();
	      break;

      case ID_EXTENSION_FINDUSEDFILE:
      case ID_EXTENSION_FINDUSEDMODULE:
         FindUsedDlgThread::Start( wParam );
         break;

      case ID_EXTENSION_PROCESSICONS:
         theApp.bShowProcessesIcons = !theApp.bShowProcessesIcons;
         theApp.SetProcessesIcons( theApp.bShowProcessesIcons );
         break;

      case ID_EXTENSION_HIDESERVICES:
         theApp.bHideServiceProcesses = !theApp.bHideServiceProcesses;
         theApp.RefreshProcessList();
         break;
         
		case ID_EXTENSION_ABOUT:
			AboutExtensionThread::Start();
			break;
			
		}
		break;

	case WM_INITMENUPOPUP:
	   {
	      HMENU hMenu = (HMENU)wParam;
	      switch ( GetMenuItemID( hMenu, 0 ) )
	      {
	      case ID_EXTENSION_FINDUSEDFILE:
	         CheckMenuItem( hMenu, ID_EXTENSION_HIDESERVICES, MF_BYCOMMAND | 
	                  ( theApp.bHideServiceProcesses ? MF_CHECKED : MF_UNCHECKED ) );
	         CheckMenuItem( hMenu, ID_EXTENSION_PROCESSICONS, MF_BYCOMMAND | 
	                  ( theApp.bShowProcessesIcons ? MF_CHECKED : MF_UNCHECKED ) );
	         break;

         case ID_EXTENSION_CPU00:
	         for ( int i = 0; i < 4; i++ )
	            CheckMenuItem( hMenu, i, MF_BYPOSITION | MF_UNCHECKED );

	         CheckMenuItem( hMenu, theApp.iProcessUsageAlertLimit /  25, MF_BYPOSITION | MF_CHECKED );
	         break;
	      }
	   }
	   
	   break;

	case WM_SETTEXT:
	   lParam = (LPARAM)theApp.GetNewTaskManagerCaption();
	   break;
	}

	return CallWindowProc( 
				theApp.fnOriginTaskManagerWndProc, 
				hwnd, 
				uMsg, 
				wParam, 
				lParam );
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
            pItem->mask |= LVIF_IMAGE;
            pItem->iImage = I_IMAGECALLBACK;
         }
      }
      break;

   case WM_KEYUP:
      if ( wParam == VK_DELETE )
      {
         
         ProcessesItemData* pData = theApp.GetSelectedProcessData();
	      if ( pData != NULL )
	         theApp.KillProcess( pData->processId );
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
	case WM_COMMAND:
	   switch( wParam )
	   {
	   case ID_PROCESSES_FILES:
	   case ID_PROCESSES_MODULES: 
	   case ID_PROCESSES_HANDLES:
	   case ID_PROCESSES_WINDOWS:
	      {
	         ProcessesItemData* pData = theApp.GetSelectedProcessData();
	         if ( pData != NULL )
	            SystemInfoDlgThread::Start( pData->processId, wParam );
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

                        RECT iconRect;
                        if ( ListView_GetItemRect( theApp.hwndProcessesList, lplvcd->nmcd.dwItemSpec, &iconRect, LVIR_ICON ) )
                        {
                           FillRect( lplvcd->nmcd.hdc, &iconRect, (HBRUSH)(COLOR_WINDOW+1) );
                        }

                        if ( pData == NULL )
	                        return CDRF_NEWFONT;

                        if ( theApp.iProcessUsageAlertLimit != 0 &&
                              pData->usageCPU > theApp.iProcessUsageAlertLimit )
                           lplvcd->clrText = theApp.clrCPUUsageAlert;
                        else 
                        if ( theApp.bHideServiceProcesses && theApp.IsProcessService( pData->processId ) )
                           lplvcd->clrText = theApp.clrServiceProcess;
                        else
                           lplvcd->clrText = theApp.clrDefaultProcess;

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
      if ( GetMenuItemID( (HMENU)wParam, 0 ) == 40028 ) //ENDPROCESS 
	   {
	      ProcessesItemData* pData = theApp.GetSelectedProcessData();
	      if ( pData != NULL && pData->processId != 0 )
      	{
      	   InsertMenu( (HMENU)wParam, 0, MF_BYPOSITION | MF_STRING, ID_PROCESSES_FILES, _T("&Files") );
            InsertMenu( (HMENU)wParam, 1, MF_BYPOSITION | MF_STRING, ID_PROCESSES_MODULES, _T("&Modules") );
            InsertMenu( (HMENU)wParam, 2, MF_BYPOSITION | MF_STRING, ID_PROCESSES_HANDLES, _T("&Handles") );
            InsertMenu( (HMENU)wParam, 3, MF_BYPOSITION | MF_STRING, ID_PROCESSES_WINDOWS, _T("&Windows") );
            InsertMenu( (HMENU)wParam, 4, MF_BYPOSITION | MF_SEPARATOR, 0, NULL );
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

                  ApplicationsItemData* pData = theApp.GetApplicationsData( pDispInfo->item.iItem );

                  if ( pDispInfo->item.mask & LVIF_TEXT )
                     switch ( pDispInfo->item.iSubItem )
                     {
                     case 2:
                        {
                           DWORD pID;

                           GetWindowThreadProcessId( pData->hWnd, &pID );

                           swprintf( (wchar_t*)pDispInfo->item.pszText, L"%d", pID );
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

TASKMANAGEREXDLL_API BOOL Initialize()
{
   return theApp.Initialize();
}

void TaskManagerExDllApp::UpdateMainMenu( HMENU hMenu )
{
   if ( GetMenuItemID( GetSubMenu( hMenu, 1 ), 0 ) == ID_EXTENSION_FINDUSEDFILE )
      return;

   HMENU hExtensionPopupMenu = CreatePopupMenu();
   HMENU hCPUUsageAlertPopupMenu = CreatePopupMenu();

   AppendMenu( hCPUUsageAlertPopupMenu, MF_BYPOSITION | MF_STRING, ID_EXTENSION_CPU00, _T("Off") );
   AppendMenu( hCPUUsageAlertPopupMenu, MF_BYPOSITION | MF_STRING, ID_EXTENSION_CPU25, _T("25%") );
   AppendMenu( hCPUUsageAlertPopupMenu, MF_BYPOSITION | MF_STRING, ID_EXTENSION_CPU50, _T("50%") );
   AppendMenu( hCPUUsageAlertPopupMenu, MF_BYPOSITION | MF_STRING, ID_EXTENSION_CPU75, _T("75%") );

	AppendMenu( hExtensionPopupMenu, MF_STRING, ID_EXTENSION_FINDUSEDFILE, _T("Find used &file") );
	AppendMenu( hExtensionPopupMenu, MF_STRING, ID_EXTENSION_FINDUSEDMODULE, _T("Find used &module") );
   AppendMenu( hExtensionPopupMenu, MF_SEPARATOR, 0, NULL );
	AppendMenu( hExtensionPopupMenu, MF_POPUP, (UINT)hCPUUsageAlertPopupMenu, _T("&CPU Usage Alert") );
	AppendMenu( hExtensionPopupMenu, MF_STRING, ID_EXTENSION_PROCESSICONS, _T("&Show process icons") );
	AppendMenu( hExtensionPopupMenu, MF_STRING, ID_EXTENSION_HIDESERVICES, _T("&Hide services") );
	AppendMenu( hExtensionPopupMenu, MF_SEPARATOR, 0, NULL );
   AppendMenu( hExtensionPopupMenu, MF_STRING, ID_EXTENSION_ABOUT, _T("&About Extension") );

   InsertMenu( hMenu, 1, MF_BYPOSITION | MF_POPUP, (int)hExtensionPopupMenu, _T("Extension") );
}

void TaskManagerExDllApp::SetProcessesIcons( BOOL bShow )
{
   ListView_SetImageList( 
            hwndProcessesList, 
            bShow ? ListView_GetImageList(hwndApplicationsList, LVSIL_SMALL ) : NULL, 
            LVSIL_SMALL  );

   int processCnt = ListView_GetItemCount( hwndProcessesList );
   for ( int nItem = 0; nItem < processCnt; nItem++ )
   {
      LVITEM lvi;
	   memset(&lvi, 0, sizeof(LVITEM));
	   lvi.iItem = nItem;
	   lvi.mask = LVIF_IMAGE;
	   lvi.iImage = bShow ? I_IMAGECALLBACK : 0;

	   ::SendMessage( hwndProcessesList, LVM_SETITEMW, 0, (LPARAM)&lvi );
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
   lvc.cx = 50;
   lvc.fmt = HDF_RIGHT;
	ListView_InsertColumn( hwndApplicationsList, 2, &lvc );

	ListView_SetExtendedListViewStyle( hwndApplicationsList, 
         ListView_GetExtendedListViewStyle( hwndApplicationsList ) |
         LVS_EX_FULLROWSELECT ) ;

	UpdateWindow( hwndApplicationsList );
}


TaskManagerExDllApp::ProcessesItemData* TaskManagerExDllApp::GetSelectedProcessData()
{
   return GetProcessData( ListView_GetNextItem( hwndProcessesList, -1, LVNI_SELECTED ) );
}

TaskManagerExDllApp::ProcessesItemData* TaskManagerExDllApp::GetProcessData( int nItem )
{
   LVITEM lvi;
	memset(&lvi, 0, sizeof(LVITEM));
	lvi.iItem = nItem;
	lvi.mask = LVIF_PARAM;

	::SendMessage( hwndProcessesList, LVM_GETITEM, 0, (LPARAM)&lvi );

	return (ProcessesItemData*)lvi.lParam;
}

TaskManagerExDllApp::ApplicationsItemData* TaskManagerExDllApp::GetApplicationsData( int nItem )
{
   LVITEM lvi;
	memset(&lvi, 0, sizeof(LVITEM));
	lvi.iItem = nItem;
	lvi.mask = LVIF_PARAM;

	::SendMessage( hwndApplicationsList, LVM_GETITEM, 0, (LPARAM)&lvi );

	return (ApplicationsItemData*)lvi.lParam;
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
	DWORD groupLength = 50;
	PTOKEN_GROUPS groupInfo = NULL;

	SID_IDENTIFIER_AUTHORITY siaNt = SECURITY_NT_AUTHORITY;
	PSID pInteractiveSid = NULL;
	PSID pServiceSid = NULL;

	DWORD dwRet = NO_ERROR;
    
    // reset flags
	BOOL isInteractive = FALSE;
	isService = FALSE;

	DWORD ndx;

	HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pID );

	// open the token
	if (!::OpenProcessToken( hProcess, TOKEN_QUERY, &hProcessToken) )
	{
		dwRet = ::GetLastError();
		goto closedown;
	}

	// allocate a buffer of default size
	groupInfo = (PTOKEN_GROUPS)::LocalAlloc(0, groupLength);
	if (groupInfo == NULL)
	{
		dwRet = ::GetLastError();
		goto closedown;
	}

	// try to get the info
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

		groupInfo = (PTOKEN_GROUPS)::LocalAlloc(0, groupLength);
		if (groupInfo == NULL)
		{
			dwRet = ::GetLastError();
			goto closedown;
		}

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
	for (ndx = 0; ndx < groupInfo->GroupCount ; ndx += 1)
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
   BOOL ret;

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
         processImageIndex = i + 1;
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

void TaskManagerExDllApp::LoadDefaultParameters()
{
   iProcessUsageAlertLimit = 0;
	clrCPUUsageAlert = RGB( 255, 0, 0 );
   clrServiceProcess = RGB( 192, 192, 192 );
   clrDefaultProcess = RGB( 0, 0, 0 );
   bShowProcessesIcons = TRUE;
   bHideServiceProcesses = TRUE;

   HKEY hKey = NULL;
   DWORD dwType = REG_DWORD;
   DWORD dwSize = sizeof(DWORD);

   ::RegCreateKey( 
         HKEY_CURRENT_USER, 
         _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\TaskManager\\Extension"), 
         &hKey );

   ::RegQueryValueEx( hKey, _T("ProcessUsageAlertLimit"), NULL, &dwType, 
               (LPBYTE)&iProcessUsageAlertLimit, &dwSize );
   ::RegQueryValueEx( hKey, _T("CPUUsageAlertColor"), NULL, &dwType,
               (LPBYTE)&clrCPUUsageAlert, &dwSize );
   ::RegQueryValueEx( hKey, _T("ServiceProcessHideColor"), NULL, &dwType,
               (LPBYTE)&clrServiceProcess, &dwSize );
   ::RegQueryValueEx( hKey, _T("ProcessColor"), NULL, &dwType,
               (LPBYTE)&clrDefaultProcess, &dwSize );
   ::RegQueryValueEx( hKey, _T("ShowProcessesIcons"), NULL, &dwType,
               (LPBYTE)&bShowProcessesIcons, &dwSize );
   ::RegQueryValueEx( hKey, _T("HideServiceProcesses"), NULL, &dwType,
               (LPBYTE)&bHideServiceProcesses, &dwSize );

   ::RegCloseKey( hKey );
}  

void TaskManagerExDllApp::SaveDefaultParameters()
{
   HKEY hKey = NULL;

   ::RegCreateKey( 
         HKEY_CURRENT_USER, 
         _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\TaskManager\\Extension"), 
         &hKey );

   ::RegSetValueEx( hKey, _T("ProcessUsageAlertLimit"), NULL, REG_DWORD, 
               (LPBYTE)&iProcessUsageAlertLimit, sizeof(DWORD) );
   ::RegSetValueEx( hKey, _T("CPUUsageAlertColor"), NULL, REG_DWORD,
               (LPBYTE)&clrCPUUsageAlert, sizeof(DWORD) );
   ::RegSetValueEx( hKey, _T("ServiceProcessHideColor"), NULL, REG_DWORD,
               (LPBYTE)&clrServiceProcess, sizeof(DWORD) );
   ::RegSetValueEx( hKey, _T("ProcessColor"), NULL, REG_DWORD,
               (LPBYTE)&clrDefaultProcess, sizeof(DWORD) );
   ::RegSetValueEx( hKey, _T("ShowProcessesIcons"), NULL, REG_DWORD,
               (LPBYTE)&bShowProcessesIcons, sizeof(DWORD) );
   ::RegSetValueEx( hKey, _T("HideServiceProcesses"), NULL, REG_DWORD,
               (LPBYTE)&bHideServiceProcesses, sizeof(DWORD) );

   ::RegCloseKey( hKey );
}
