// SystemInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskManagerExDll.h"
#include "SystemInfo.h"
#include "SystemInfoDlg.h"
#include "DllVersion.h"
#include "LoadDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE( SystemInfoDlgThread, CWinThread )

BOOL SystemInfoDlgThread::InitInstance()
{
   SystemInfoDlg dlg;//( CWnd::FromHandle( hwndTaskManager ) );
   m_pMainWnd = &dlg;

   dlg.DoModal();

   return FALSE;
}

BOOL SystemInfoDlgThread::Initialize( DWORD pID, WPARAM wCommand )
{
   m_processID = pID;
   m_command = wCommand;

   return TRUE;
}

SystemInfoDlgThread* SystemInfoDlgThread::Start( DWORD pID, WPARAM wCommand )
{
   SystemInfoDlgThread* _this = (SystemInfoDlgThread*)::AfxBeginThread( 
                  RUNTIME_CLASS(SystemInfoDlgThread),
                  THREAD_PRIORITY_NORMAL,
                  0,
                  CREATE_SUSPENDED );

   _this->Initialize( pID, wCommand );

   _this->ResumeThread();

   return _this;
}

/////////////////////////////////////////////////////////////////////////////
// SystemInfoDlg dialog

SystemInfoDlg::SystemInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SystemInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SystemInfoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void SystemInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SystemInfoDlg)
	DDX_Control(pDX, IDC_SYSTEMINFO, m_SystemInfoList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SystemInfoDlg, CDialog)
	//{{AFX_MSG_MAP(SystemInfoDlg)
	ON_WM_SIZE()
	ON_COMMAND(ID_EXTENSION_REFRESH, OnSysteminfodlgRefresh)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_HANDLES_RELEASE, OnHandlesRelease)
	ON_COMMAND(ID_MODULES_UNLOAD, OnModulesUnload)
	ON_COMMAND(ID_WINDOWS_ACTIVATE, OnWindowsActivate)
	ON_COMMAND(ID_WINDOWS_CLOSE, OnWindowsClose)
	ON_COMMAND(ID_HANDLES_EVENT_SET, OnHandlesEventSet)
	ON_COMMAND(ID_HANDLES_EVENT_RESET, OnHandlesEventReset)
	ON_COMMAND(ID_HANDLES_EVENT_PULSE, OnHandlesEventPulse)
	ON_COMMAND(ID_HANDLES_MUTANT_RELEASE, OnHandlesMutantRelease)
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SystemInfoDlg message handlers

BOOL SystemInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

   ::SetClassLong( GetSafeHwnd(), GCL_HICON, (LONG)theApp.hMainIcon );
   //SetIcon( theApp.hMainIcon, FALSE );

   CreateToolbar();

   Resize();

   RefreshAll();

   ResizeToListCtrl();

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL SystemInfoDlg::CreateToolbar()
{
   UINT toolbarID = IDR_SYSTEMINFODLG;

   if ( !m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(toolbarID))
		return TRUE;
   
   // We need to resize the dialog to make room for control bars.
   // First, figure out how big the control bars are.
   CRect rcClientStart;
   CRect rcClientNow;
   GetClientRect(rcClientStart);
   RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 
   				0, reposQuery, rcClientNow);
   
   // Now move all the controls so they are in the same relative
   // position within the remaining client area as they would be
   // with no control bars.
   CPoint ptOffset(rcClientNow.left - rcClientStart.left,
   				rcClientNow.top - rcClientStart.top); 

   CRect  rcChild;					
   CWnd* pwndChild = GetWindow(GW_CHILD);
   while (pwndChild)
   {                               
   	pwndChild->GetWindowRect(rcChild);
   	ScreenToClient(rcChild);
   	rcChild.OffsetRect(ptOffset);
   	pwndChild->MoveWindow(rcChild, FALSE);
   	pwndChild = pwndChild->GetNextWindow();
   }

   // Adjust the dialog window dimensions
   CRect rcWindow;
   GetWindowRect(rcWindow);
   rcWindow.right += rcClientStart.Width() - rcClientNow.Width();
   rcWindow.bottom += rcClientStart.Height() - rcClientNow.Height();
   MoveWindow(rcWindow, FALSE);
   
   // And position the control bars
   RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

   return TRUE;
}

void SystemInfoDlg::RefreshAll()
{
   SystemInfoDlgThread* pThread = (SystemInfoDlgThread*)AfxGetThread();

   int nColumnCount = m_SystemInfoList.GetHeaderCtrl()->GetItemCount();

   // Delete all of the columns.
   for (int i=0;i < nColumnCount;i++)
   {
      m_SystemInfoList.DeleteColumn(0);
   }

   CString strCaption;

   switch ( pThread->m_command )
   {
   case ID_PROCESSES_FILES:
   case ID_PROCESSES_MODULES:
   case ID_PROCESSES_WINDOWS:
   case ID_PROCESSES_HANDLES:
      strCaption.Format( _T("PID = %d(0x%X) - "), pThread->m_processID, pThread->m_processID );
      break;
   }

   // Columns
   switch ( pThread->m_command )
   {
   case ID_PROCESSES_FILES:
	  strCaption += _T("Files");

	  m_SystemInfoList.InsertColumn( 0, _T("Handle") );
	  m_SystemInfoList.InsertColumn( 1, _T("Name") );
     m_SystemInfoList.InsertColumn( 2, _T("Path") );
     m_SystemInfoList.InsertColumn( 3, _T("Device") );
	  break;

   case ID_PROCESSES_MODULES:
      strCaption += _T("Modules");

      m_SystemInfoList.InsertColumn( 0, _T("Handle") );
      m_SystemInfoList.InsertColumn( 1, _T("Name") );
      m_SystemInfoList.InsertColumn( 2, _T("Size"), LVCFMT_RIGHT );
      m_SystemInfoList.InsertColumn( 3, _T("Version") );
      m_SystemInfoList.InsertColumn( 4, _T("Path") );
      break;

   case ID_PROCESSES_WINDOWS:
	  strCaption += _T("Windows");

      m_SystemInfoList.InsertColumn( 0, _T("Handle") );
      m_SystemInfoList.InsertColumn( 1, _T("Caption") );
      m_SystemInfoList.InsertColumn( 2, _T("Class") );
      m_SystemInfoList.InsertColumn( 3, _T("Rect") );
      m_SystemInfoList.InsertColumn( 4, _T("Style") );
      m_SystemInfoList.InsertColumn( 5, _T("ExStyle") );
      
      break;

   case ID_PROCESSES_HANDLES:
      strCaption += _T("Handles");

      m_SystemInfoList.InsertColumn( 0, _T("Handle") );
      m_SystemInfoList.InsertColumn( 1, _T("Type") );
      m_SystemInfoList.InsertColumn( 2, _T("Name") );
      break;
   }

   SetWindowText( strCaption );

   RefreshList();
}

void SystemInfoDlg::RefreshList()
{
   SystemInfoDlgThread* pThread = (SystemInfoDlgThread*)AfxGetThread();

   if ( !theApp.IsProcessIdValid( pThread->m_processID ) )
   {
      if ( AfxMessageBox( _T("Probably the process ID is not valid anymore. Do you want to close the process window?"),
	         MB_YESNO ) == IDYES )
		   EndDialog(0);
	   return;
   }

   m_SystemInfoList.SetRedraw( FALSE );

   m_SystemInfoList.DeleteAllItems();

   // List items
   switch ( pThread->m_command )
   {
   case ID_PROCESSES_FILES:
      {
         SystemHandleInformation fi( pThread->m_processID, TRUE, _T("File") );

	      for ( POSITION pos = fi.m_HandleInfos.GetHeadPosition(); pos != NULL; )
         {
            CString strItem;
            CString strTemp;

            SystemHandleInformation::SYSTEM_HANDLE& hi = fi.m_HandleInfos.GetNext(pos);

            strItem.Format( _T("0x%08X"), hi.HandleNumber );

            int nPos = m_SystemInfoList.InsertItem( 0, strItem );

            strItem = _T("");
            SystemHandleInformation::GetName( (HANDLE)hi.HandleNumber, strItem, pThread->m_processID );
            m_SystemInfoList.SetItemText( nPos, 3, strItem );

            for ( int i = strItem.GetLength() - 1; i >= 0; i-- )
            {
               if ( strItem[i] == _T('\\') )
                  break;

               strTemp = strItem[i] + strTemp;
            }
            m_SystemInfoList.SetItemText( nPos, 1, strTemp );

            strTemp = _T("");
            SystemInfoUtils::GetFsFileName( strItem, strTemp );
            m_SystemInfoList.SetItemText( nPos, 2, strTemp );
         }
		 }
      break;

   case ID_PROCESSES_MODULES:
      {
         SystemModuleInformation mi( pThread->m_processID, TRUE );

		 for ( POSITION pos = mi.m_ModuleInfos.GetHeadPosition(); pos != NULL; )
         {
            CString strItem;
            SystemModuleInformation::MODULE_INFO& m = mi.m_ModuleInfos.GetNext(pos);

            strItem.Format( _T("0x%08X"), m.Handle );

            int nPos = m_SystemInfoList.InsertItem( 0, strItem );

            m_SystemInfoList.SetItemText( nPos, 1, m.Name );

            // Get size
            CFile file;
            strItem = _T("");
            if ( file.Open( m.FullPath, CFile::modeRead ) )
            {
               strItem.Format( _T("%d"), file.GetLength() );
               file.Close();
            }
            m_SystemInfoList.SetItemText( nPos, 2, strItem );

            strItem = _T("");
            GetModuleVersion( m.FullPath, strItem );
            m_SystemInfoList.SetItemText( nPos, 3, strItem );

            m_SystemInfoList.SetItemText( nPos, 4, m.FullPath );
         }
      }
      break;

   case ID_PROCESSES_WINDOWS:
      {
         SystemWindowInformation wi( pThread->m_processID, TRUE );

		 for ( POSITION pos = wi.m_WindowInfos.GetHeadPosition(); pos != NULL; )
         {
            TCHAR szBuffer[0x100];
            CString strItem;
            SystemWindowInformation::WINDOW_INFO& w = wi.m_WindowInfos.GetNext(pos);

            strItem.Format( _T("0x%08X"), w.hWnd );

            int nPos = m_SystemInfoList.InsertItem( 0, strItem );

            m_SystemInfoList.SetItemText( nPos, 1, w.Caption );

            strItem = _T("");
            if ( ::GetClassName( w.hWnd, szBuffer, 0x100 ) )
               strItem = szBuffer;

            m_SystemInfoList.SetItemText( nPos, 2, strItem );

            CRect rect;
            strItem = _T("");
            if ( ::GetWindowRect( w.hWnd, &rect ) )
               strItem.Format( _T("(%d,%d)-(%d,%d) %dx%d"), 
                  rect.left, rect.top,
                  rect.right, rect.bottom,
                  rect.Width(), rect.Height() );
            m_SystemInfoList.SetItemText( nPos, 3, strItem );

            strItem.Format( _T("0x%08X"), ::GetWindowLong( w.hWnd, GWL_STYLE ) );
            m_SystemInfoList.SetItemText( nPos, 4, strItem );

            strItem.Format( _T("0x%08X"), ::GetWindowLong( w.hWnd, GWL_EXSTYLE ) );
            m_SystemInfoList.SetItemText( nPos, 5, strItem );
         }
      }
      break;

   case ID_PROCESSES_HANDLES:
      {
         SystemHandleInformation hi( pThread->m_processID, TRUE );

		 for ( POSITION pos = hi.m_HandleInfos.GetHeadPosition(); pos != NULL; )
         {
            CString strItem;
            SystemHandleInformation::SYSTEM_HANDLE& h = hi.m_HandleInfos.GetNext(pos);

            strItem.Format( _T("0x%08X"), h.HandleNumber );

            int nPos = m_SystemInfoList.InsertItem( 0, strItem );

            SystemHandleInformation::GetTypeToken( (HANDLE)h.HandleNumber, strItem, pThread->m_processID );

            m_SystemInfoList.SetItemText( nPos, 1, strItem );

   			SystemHandleInformation::GetName( (HANDLE)h.HandleNumber, strItem, pThread->m_processID );

            m_SystemInfoList.SetItemText( nPos, 2, strItem );
         }
      }
      break;
   }

   ResizeColumns();

   m_SystemInfoList.SetRedraw( TRUE );

   m_SystemInfoList.Invalidate();

   return;
}

void SystemInfoDlg::Resize()
{
   CRect rect;
   CRect toolbarRect;

   GetClientRect( &rect );

   if ( m_wndToolBar.GetSafeHwnd() != NULL )
   {
      m_wndToolBar.GetWindowRect( toolbarRect );

      RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
   }

	if ( m_SystemInfoList.GetSafeHwnd() != NULL )
	{
		m_SystemInfoList.SetWindowPos( NULL, 0, 0, rect.Width(), rect.Height() - toolbarRect.Height(), SWP_NOMOVE );
	}
}

void SystemInfoDlg::ResizeColumns() 
{
   int colCount = m_SystemInfoList.GetHeaderCtrl()->GetItemCount();
   
   for ( int col = 0; col < colCount; col++) 
   {
      m_SystemInfoList.SetColumnWidth( col, LVSCW_AUTOSIZE );

      m_SystemInfoList.SetColumnWidth( col, max( 60, m_SystemInfoList.GetColumnWidth( col ) ) );
   }
  
}

void SystemInfoDlg::ResizeToListCtrl()
{
   CRect rect;

   GetWindowRect( &rect );
   int width = 30;
   int height = rect.Height();

   int screenWidth = 800;
   if ( SystemParametersInfo( SPI_GETWORKAREA, 0, &rect, 0 ) )
      screenWidth = rect.Width();

   int colCount = m_SystemInfoList.GetHeaderCtrl()->GetItemCount();
   
   for ( int col = 0; col < colCount; col++) 
      width += m_SystemInfoList.GetColumnWidth( col );

   SetWindowPos( NULL, 0, 0, max( 350, min( width, screenWidth / 2 ) ), height, SWP_NOMOVE );
}

void SystemInfoDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

   Resize();
}

void SystemInfoDlg::OnSysteminfodlgRefresh() 
{
	RefreshList();
}

void SystemInfoDlg::OnContextMenu(CWnd* pWnd, CPoint ) 
{
   if ( pWnd == &m_SystemInfoList )
   {
      SystemInfoDlgThread* pThread = (SystemInfoDlgThread*)AfxGetThread();
      CMenu popupMenu;

      popupMenu.LoadMenu( MAKEINTRESOURCE(IDR_PROCESSES) );

      CPoint point;
      GetCursorPos( &point );

      switch ( pThread->m_command )
      {
      case ID_PROCESSES_MODULES:
         popupMenu.GetSubMenu(0)->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, this, NULL );
         break;
         
      case ID_PROCESSES_WINDOWS:
         popupMenu.GetSubMenu(1)->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, this, NULL );
         break;

      case ID_PROCESSES_FILES:
      case ID_PROCESSES_HANDLES:
         {
            CString subItemText;
            CMenu* pPopupMenu = popupMenu.GetSubMenu(2);

            if ( m_SystemInfoList.GetSelectedSubItemText( 1, subItemText ) )
            {
               int nPos = -1;
 
               if ( subItemText == _T("Event") )
                  nPos = 2;
               else
               if ( subItemText == _T("Mutant") )
                  nPos = 3;

               if ( nPos != -1)
                  pPopupMenu->EnableMenuItem( nPos, MF_BYPOSITION | MF_ENABLED );
            }

            pPopupMenu->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, this, NULL );
         }
         break;
      }
   }
}

ULONG SystemInfoDlg::GetSelectedHandle( int nCol )
{
   CString subItemText;
   ULONG handle = 0;

   if ( !m_SystemInfoList.GetSelectedSubItemText( nCol, subItemText ) )
      return 0;

   _stscanf( (LPCTSTR)subItemText, _T("0x%X"), &handle );

   return handle;
}

ULONG SystemInfoDlg::GetSelectedHandle()
{
   CString subItemText;
   ULONG handle = 0;

   if ( !m_SystemInfoList.GetSelectedSubItemText( _T("Handle"), subItemText ) )
      return 0;

   _stscanf( (LPCTSTR)subItemText, _T("0x%X"), &handle );

   return handle;
}

void SystemInfoDlg::OnHandlesRelease() 
{
   SystemInfoDlgThread* pThread = (SystemInfoDlgThread*)AfxGetThread();
   HANDLE handle = (HANDLE)GetSelectedHandle();
   DWORD processID = pThread->m_processID;

   if ( processID == 0 )
      return;

   if ( handle != NULL )
	{
      if ( RemoteSimpleHandleFunction( processID, handle, "CloseHandle" ) )
         Sleep( 1000 );
      else
         AfxMessageBox( _T("Couldn't release handle") );

         RefreshList();
   }
}

void SystemInfoDlg::OnModulesUnload() 
{
   SystemInfoDlgThread* pThread = (SystemInfoDlgThread*)AfxGetThread();
	CString  modulePath;
	DWORD processID = pThread->m_processID;

   if ( processID == 0 )
      return;
	
	if ( !m_SystemInfoList.GetSelectedSubItemText( _T("Path"), modulePath ) )
      return;

   LoadDllForRemoteThread( processID, FALSE, TRUE, (LPCTSTR)modulePath, NULL, 
	      NULL, NULL, NULL, NULL );

   RefreshList();
}

void SystemInfoDlg::OnWindowsActivate() 
{
   HWND hWnd = (HWND)GetSelectedHandle( 0 );

   if ( hWnd )
      ::SetActiveWindow( hWnd );
}

void SystemInfoDlg::OnWindowsClose() 
{
   HWND hWnd = (HWND)GetSelectedHandle( 0 );

   if ( hWnd )
   {
      ::SendMessage( hWnd, WM_CLOSE, 0, 0 );

      RefreshList();
   }
}

void SystemInfoDlg::OnHandlesEventSet() 
{
	SystemInfoDlgThread* pThread = (SystemInfoDlgThread*)AfxGetThread();
   HANDLE handle = (HANDLE)GetSelectedHandle( 0 );

   if ( handle != NULL )
      RemoteSimpleHandleFunction( pThread->m_processID, handle, "SetEvent" );
}

void SystemInfoDlg::OnHandlesEventReset() 
{
	SystemInfoDlgThread* pThread = (SystemInfoDlgThread*)AfxGetThread();
   HANDLE handle = (HANDLE)GetSelectedHandle( 0 );

   if ( handle != NULL )
      RemoteSimpleHandleFunction( pThread->m_processID, handle, "ResetEvent" );
}

void SystemInfoDlg::OnHandlesEventPulse() 
{
	SystemInfoDlgThread* pThread = (SystemInfoDlgThread*)AfxGetThread();
   HANDLE handle = (HANDLE)GetSelectedHandle( 0 );

   if ( handle != NULL )
      RemoteSimpleHandleFunction( pThread->m_processID, handle, "PulseEvent" );
}

void SystemInfoDlg::OnHandlesMutantRelease() 
{
	SystemInfoDlgThread* pThread = (SystemInfoDlgThread*)AfxGetThread();
   HANDLE handle = (HANDLE)GetSelectedHandle( 0 );

   if ( handle != NULL )
      RemoteSimpleHandleFunction( pThread->m_processID, handle, "ReleaseMutex" );
}

void SystemInfoDlg::OnOK()
{
}

void SystemInfoDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
	
	Resize();
}
