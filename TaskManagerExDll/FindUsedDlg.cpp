// FindUsedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskManagerExDll.h"
#include "SystemInfo.h"
#include "FindUsedDlg.h"
#include "DllVersion.h"
#include "LoadDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE( FindUsedDlgThread, CWinThread )

BOOL FindUsedDlgThread::InitInstance()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   FindUsedDlg dlg;//( CWnd::FromHandle( hwndTaskManager ) );
   m_pMainWnd = &dlg;

   dlg.DoModal();

   return FALSE;
}

BOOL FindUsedDlgThread::Initialize( WPARAM wCommand )
{
   m_command = wCommand;

   return TRUE;
}

FindUsedDlgThread* FindUsedDlgThread::Start( WPARAM wCommand )
{
   FindUsedDlgThread* _this = (FindUsedDlgThread*)::AfxBeginThread( 
                  RUNTIME_CLASS(FindUsedDlgThread),
                  THREAD_PRIORITY_NORMAL,
                  0,
                  CREATE_SUSPENDED );

   _this->Initialize( wCommand );

   _this->ResumeThread();

   return _this;
}

/////////////////////////////////////////////////////////////////////////////
// FindUsedDlg dialog

FindUsedDlg::FindUsedDlg(CWnd* pParent /*=NULL*/)
	: CDialog(FindUsedDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(FindUsedDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void FindUsedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FindUsedDlg)
	DDX_Control(pDX, IDC_FINDUSED, m_SystemInfoList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FindUsedDlg, CDialog)
	//{{AFX_MSG_MAP(FindUsedDlg)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_HANDLES_RELEASE, OnHandlesRelease)
	ON_COMMAND(ID_MODULES_UNLOAD, OnModulesUnload)
	ON_COMMAND(ID_EXTENSION_REFRESH, OnExtensionRefresh)
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FindUsedDlg message handlers

BOOL FindUsedDlg::OnInitDialog() 
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

BOOL FindUsedDlg::CreateToolbar()
{
   FindUsedDlgThread* pThread = (FindUsedDlgThread*)AfxGetThread();

   UINT toolbarID = IDR_FINDUSEDDLG;

   if ( !m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(toolbarID))
		return TRUE;

    CRect rect;
    CString staticText = pThread->m_command == ID_EXTENSION_FINDUSEDFILE ? 
               _T("File name:") : 
               _T("Module name:");

    m_wndToolBar.SetButtonInfo( 0, 0, TBBS_SEPARATOR, GetDC()->GetTextExtent( staticText ).cx);
    m_wndToolBar.SetButtonInfo( 1, 0, TBBS_SEPARATOR, 120);

    m_wndToolBar.GetItemRect( 0, &rect);
    rect.top += 2;
    rect.left += 5;

    if ( !m_fileNameStaticCtrl.Create( 
            staticText,
            WS_CHILD | WS_VISIBLE, rect, &m_wndToolBar ) )
       return FALSE;
    
    m_wndToolBar.GetItemRect( 1, &rect);
    if (!m_fileNameCtrl.Create( WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, rect, &m_wndToolBar, 1987 ))
       return FALSE;

    m_fileNameCtrl.ModifyStyleEx( 0, WS_EX_CLIENTEDGE );

    m_fileNameStaticCtrl.SetFont( GetFont() );
    m_fileNameCtrl.SetFont( GetFont() );

    m_fileNameStaticCtrl.ShowWindow(SW_SHOW);
    m_fileNameCtrl.ShowWindow(SW_SHOW);

    m_fileNameCtrl.SetFocus();
   
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

void FindUsedDlg::RefreshAll()
{
   FindUsedDlgThread* pThread = (FindUsedDlgThread*)AfxGetThread();

   int nColumnCount = m_SystemInfoList.GetHeaderCtrl()->GetItemCount();

   m_SystemInfoList.DeleteAllItems();

   // Delete all of the columns.
   for (int i=0;i < nColumnCount;i++)
   {
      m_SystemInfoList.DeleteColumn(0);
   }

   CString strCaption;

   switch ( pThread->m_command )
   {
   case ID_EXTENSION_FINDUSEDFILE:
      strCaption.Format( _T("Find used file") );
      break;

   case ID_EXTENSION_FINDUSEDMODULE:
      strCaption.Format( _T("Find used module") );
      break;
   }

   // Columns
   switch ( pThread->m_command )
   {
   case ID_EXTENSION_FINDUSEDFILE:
      m_SystemInfoList.InsertColumn( 0, _T("Handle") );
      m_SystemInfoList.InsertColumn( 1, _T("PID"), LVCFMT_RIGHT );
      m_SystemInfoList.InsertColumn( 2, _T("Name") );
      m_SystemInfoList.InsertColumn( 3, _T("Path") );
      m_SystemInfoList.InsertColumn( 4, _T("Device") );
      break;

   case ID_EXTENSION_FINDUSEDMODULE:
      m_SystemInfoList.InsertColumn( 0, _T("Handle") );
      m_SystemInfoList.InsertColumn( 1, _T("PID"), LVCFMT_RIGHT );
      m_SystemInfoList.InsertColumn( 2, _T("Name") );
      m_SystemInfoList.InsertColumn( 3, _T("Path") );
      break;
   }

   SetWindowText( strCaption );

   RefreshList();
}

void FindUsedDlg::RefreshList()
{
   FindUsedDlgThread* pThread = (FindUsedDlgThread*)AfxGetThread();

   m_SystemInfoList.SetRedraw( FALSE );

   //m_SystemInfoList.DeleteAllItems();

   // List items
   switch ( pThread->m_command )
   {
   case ID_EXTENSION_FINDUSEDFILE:
      {
         BOOL bAll;
         CString findFileName;
         m_fileNameCtrl.GetWindowText( findFileName );

         if ( findFileName == _T("") )
            break;

         bAll = findFileName == _T("*");

         SystemHandleInformation fi( (DWORD)-1, TRUE, _T("File") );

	      for ( POSITION pos = fi.m_HandleInfos.GetHeadPosition(); pos != NULL; )
         {
            CString strItem;
            CString strTemp;

            SystemHandleInformation::SYSTEM_HANDLE& hi = fi.m_HandleInfos.GetNext(pos);

            SystemHandleInformation::GetName( (HANDLE)hi.HandleNumber, strItem, hi.ProcessID );

            for ( int i = strItem.GetLength() - 1; i >= 0; i-- )
            {
               if ( strItem[i] == _T('\\') )
                  break;

               strTemp = strItem[i] + strTemp;
            }

            if ( !FreeSearchInString( strItem, findFileName ) && !bAll )
               continue;

            strTemp.Format( _T("0x%08X"), hi.HandleNumber );
            int nPos = m_SystemInfoList.InsertItem( 0, strTemp );

            strTemp.Format( _T("%d"), hi.ProcessID );
            m_SystemInfoList.SetItemText( nPos, 1, strTemp );

            m_SystemInfoList.SetItemText( nPos, 4, strItem );

            strTemp = _T("");
            for ( i = strItem.GetLength() - 1; i >= 0; i-- )
            {
               if ( strItem[i] == _T('\\') )
                  break;

               strTemp = strItem[i] + strTemp;
            }
            m_SystemInfoList.SetItemText( nPos, 2, strTemp );

            strTemp = _T("");
            SystemInfoUtils::GetFsFileName( strItem, strTemp );
            m_SystemInfoList.SetItemText( nPos, 3, strTemp );
         }
      }
      break;

   case ID_EXTENSION_FINDUSEDMODULE:
      {
         BOOL bAll;
         CString findFileName;
         m_fileNameCtrl.GetWindowText( findFileName );

         if ( findFileName == _T("") )
            break;

         bAll = findFileName == _T("*");

         SystemModuleInformation mi( (DWORD)-1, TRUE );

         for ( POSITION pos = mi.m_ModuleInfos.GetHeadPosition(); pos != NULL; )
         {
            CString strItem;
            SystemModuleInformation::MODULE_INFO& m = mi.m_ModuleInfos.GetNext(pos);

            if ( !FreeSearchInString( m.FullPath, findFileName ) && !bAll )
               continue;

            strItem.Format( _T("0x%08X"), m.Handle );
            int nPos = m_SystemInfoList.InsertItem( 0, strItem );

            strItem.Format( _T("%d"), m.ProcessId );
            m_SystemInfoList.SetItemText( nPos, 1, strItem );

            m_SystemInfoList.SetItemText( nPos, 2, m.Name );

            m_SystemInfoList.SetItemText( nPos, 3, m.FullPath );
         }
      }
      break;
   }

   ResizeColumns();

   m_SystemInfoList.SetRedraw( TRUE );

   m_SystemInfoList.Invalidate();

   ResizeToListCtrl();

   m_fileNameCtrl.SetSel( 0, -1 );
   m_fileNameCtrl.SetFocus();
   
   return;
}

void FindUsedDlg::Resize()
{
   FindUsedDlgThread* pThread = (FindUsedDlgThread*)AfxGetThread();

   CRect rect;
   CRect toolbarRect;

   GetClientRect( &rect );

   if ( m_wndToolBar.GetSafeHwnd() != NULL )
   {
      m_wndToolBar.GetWindowRect( toolbarRect );

      if ( pThread->m_command == ID_EXTENSION_FINDUSEDFILE || pThread->m_command == ID_EXTENSION_FINDUSEDMODULE)
      {
         CRect rect1;
         CRect rect3;
         int w;
         int h;

         m_wndToolBar.GetItemRect( 0, &rect1 );
         m_wndToolBar.GetItemRect( 2, &rect3 );

         w = rect.Width() - rect1.Width() - rect3.Width();
         h = toolbarRect.Height() - 2;

         m_wndToolBar.SetButtonInfo( 1, 0, TBBS_SEPARATOR, w );
         m_fileNameCtrl.SetWindowPos( NULL, 0, 0, w, h, SWP_NOMOVE );
      }
    
      RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
   }

	if ( m_SystemInfoList.GetSafeHwnd() != NULL )
	{
		m_SystemInfoList.SetWindowPos( NULL, 0, 0, rect.Width(), rect.Height() - toolbarRect.Height(), SWP_NOMOVE );
	}
}

void FindUsedDlg::ResizeColumns() 
{
   int colCount = m_SystemInfoList.GetHeaderCtrl()->GetItemCount();
   
   for ( int col = 0; col < colCount; col++) 
   {
      m_SystemInfoList.SetColumnWidth( col, LVSCW_AUTOSIZE );

      m_SystemInfoList.SetColumnWidth( col, max( 60, m_SystemInfoList.GetColumnWidth( col ) ) );
   }
  
}

void FindUsedDlg::ResizeToListCtrl()
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

void FindUsedDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

   Resize();
}

void FindUsedDlg::OnContextMenu(CWnd* pWnd, CPoint ) 
{
   if ( pWnd == &m_SystemInfoList )
   {
      FindUsedDlgThread* pThread = (FindUsedDlgThread*)AfxGetThread();
      CMenu popupMenu;

      popupMenu.LoadMenu( MAKEINTRESOURCE(IDR_PROCESSES) );

      CPoint point;
      GetCursorPos( &point );

      switch ( pThread->m_command )
      {
      case ID_EXTENSION_FINDUSEDMODULE:
         popupMenu.GetSubMenu(0)->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, this, NULL );
         break;

      case ID_EXTENSION_FINDUSEDFILE:
         popupMenu.GetSubMenu(2)->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, this, NULL );
         break;
      }
   }
}

ULONG FindUsedDlg::GetSelectedHandle( int nCol )
{
   CString subItemText;
   ULONG handle = 0;

   if ( !m_SystemInfoList.GetSelectedSubItemText( nCol, subItemText ) )
      return 0;

   _stscanf( (LPCTSTR)subItemText, _T("0x%X"), &handle );

   return handle;
}

ULONG FindUsedDlg::GetSelectedHandle()
{
   CString subItemText;
   ULONG handle = 0;

   if ( !m_SystemInfoList.GetSelectedSubItemText( _T("Handle"), subItemText ) )
      return 0;

   _stscanf( (LPCTSTR)subItemText, _T("0x%X"), &handle );

   return handle;
}

void FindUsedDlg::OnHandlesRelease() 
{
   HANDLE handle = (HANDLE)GetSelectedHandle();
   DWORD processID = 0;

   CString subItemText;
   if ( m_SystemInfoList.GetSelectedSubItemText( _T("PID"), subItemText ) )
   {
      processID = _ttoi( subItemText );
   }

   if ( processID == 0 )
      return;

   if ( handle != NULL )
	{
      if ( RemoteSimpleHandleFunction( processID, handle, "CloseHandle" ) )
         Sleep( 1000 );
      else
         AfxMessageBox( _T("Couldn't release handle") );
   }
}

void FindUsedDlg::OnModulesUnload() 
{
	CString  modulePath;
	DWORD processID = 0;

   CString subItemText;
   if ( m_SystemInfoList.GetSelectedSubItemText( _T("PID"), subItemText ) )
   {
      processID = _ttoi( subItemText );
   }

   if ( processID == 0 )
      return;
	
	if ( !m_SystemInfoList.GetSelectedSubItemText( _T("Path"), modulePath ) )
      return;

   LoadDllForRemoteThread( processID, FALSE, TRUE, (LPCTSTR)modulePath, NULL, 
	      NULL, NULL, NULL, NULL );
}

BOOL FindUsedDlg::PreTranslateMessage(MSG* pMsg) 
{
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
	   RefreshAll();
	   return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL FindUsedDlg::FreeSearchInString( LPCTSTR lpszString, LPCTSTR lpszSubString )
{
   if ( lpszSubString[0] == _T('\0') )
      return FALSE;
      
   CString szString( lpszString );
   CString szSubString( lpszSubString );

   szString.MakeUpper();
   szSubString.MakeUpper();

   return _tcsstr( szString, szSubString ) != NULL;
}

void FindUsedDlg::OnExtensionRefresh() 
{
	RefreshAll();
}

void FindUsedDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

   Resize();	
}
