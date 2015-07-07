// CSystemInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskManagerExDll.h"
#include "SystemInfo.h"
#include "SystemInfoUI.h"
#include "SystemInfoDlg.h"
#include "LoadDll.h"
#include <tlhelp32.h>
#include "Localization.h"
#include "AboutExtension.h"

#include "../AccessMaster/SecurityInformation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char g_szStamp_Mark[] = "\r\n\r\nSystemInfoDlg.cpp Timestamp: " __DATE__ ", " __TIME__ "\r\n\r\n";

#define RUN_SYSTEM_INFO_DLG_IN_SEPARATE_THREAD

/////////////////////////////////////////////////////////////////////////////

#define REASON_BASE					12340

#define REASON_SIMPLE_FUNC			REASON_BASE + 1
#define REASON_REMOTE_THREAD		REASON_BASE + 2
#define REASON_SEND_MESSAGE			REASON_BASE + 3
#define REASON_TERMINATE_PROCESS	REASON_BASE + 4
#define REASON_TERMINATE_THREAD		REASON_BASE + 5

/////////////////////////////////////////////////////////////////////////////

void MyWait( DWORD dwReason )
{
	switch( dwReason )
	{
	case REASON_SIMPLE_FUNC:
		Sleep( 100 );
		break;

	case REASON_REMOTE_THREAD:
		Sleep( 100 );
		break;

	case REASON_SEND_MESSAGE:
		Sleep( 100 );
		break;

	case REASON_TERMINATE_PROCESS:
		Sleep( 300 );
		break;

	case REASON_TERMINATE_THREAD:
		Sleep( 100 );
		break;

	default:
		ASSERT(FALSE);
	}
}

static UINT BASED_CODE indicators[] =
{
	ID_INDICATOR_PANE1,
	ID_INDICATOR_PANE2
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE( CSystemInfoDlgThread, CWinThread )

BOOL CSystemInfoDlgThread::InitInstance()
{
	SetThreadNativeLanguage();

#ifdef RUN_SYSTEM_INFO_DLG_IN_SEPARATE_THREAD
	CSystemInfoDlg dlg; //( CWnd::FromHandle( theApp.hwndTaskManager ) );
	m_pMainWnd = &dlg;

	dlg.initInfo = initInfo;

	dlg.DoModal();
#endif

	PostQuitMessage( 0 ); // quit thread gracefully
	return TRUE;
//	return FALSE; // return error to quit thread
}

CSystemInfoDlgThread* CSystemInfoDlgThread::Start( DWORD pID, WPARAM wCommand, BOOL bFind, BOOL bMultiProcess )
{
	INIT_SYSTEM_INFO	initInfo;
	initInfo.m_processID = pID;
	initInfo.m_command = wCommand;
	initInfo.m_bFind = bFind;
	initInfo.m_bMultiProcess = bMultiProcess;

#ifdef RUN_SYSTEM_INFO_DLG_IN_SEPARATE_THREAD
	CSystemInfoDlgThread* _this = (CSystemInfoDlgThread*)::AfxBeginThread(
				  RUNTIME_CLASS(CSystemInfoDlgThread),
				  THREAD_PRIORITY_NORMAL,
				  0,
				  CREATE_SUSPENDED );

	_this->initInfo = initInfo;

	_this->ResumeThread();

	return _this;
#else
	SetThreadNativeLanguage();
	CSystemInfoDlg* pDlg = new CSystemInfoDlg; //( CWnd::FromHandle( theApp.hwndTaskManager ) );

	pDlg->initInfo = initInfo;

//	pDlg->DoModal();
	CWnd* pWnd = CWnd::GetDesktopWindow(); // CWnd::FromHandle( NULL );
	pDlg->Create( CSystemInfoDlg::IDD, pWnd );
	pDlg->ShowWindow( SW_SHOW );
	pDlg->UpdateWindow();

	return NULL;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CSystemInfoDlg dialog

CSystemInfoDlg::CSystemInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSystemInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSystemInfoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hAccelTable = LocLoadAccelerators( MAKEINTRESOURCE(IDR_SYSTEM_INFO_ACCELERATOR) );
	m_bFirstFind = TRUE;
	m_bResizeListDuringRefresh = FALSE;
	m_bExpandRegions = FALSE;

	TRACE( _T("TaskManagerEx: CSystemInfoDlg::CSystemInfoDlg()\n") );
}


CSystemInfoDlg::~CSystemInfoDlg()
{
	TRACE( _T("TaskManagerEx: CSystemInfoDlg::~CSystemInfoDlg()\n") );
}

void CSystemInfoDlg::PostNcDestroy() 
{
	TRACE( _T("TaskManagerEx: CSystemInfoDlg::PostNcDestroy()\n") );

#ifdef RUN_SYSTEM_INFO_DLG_IN_SEPARATE_THREAD
	CDialog::PostNcDestroy();
#else
	// Free the C++ class.
	delete this;
#endif
}

void CSystemInfoDlg::OnClose()
{
	TRACE( _T("TaskManagerEx: CSystemInfoDlg::OnClose()\n") );

	CDialog::OnClose();

//#ifdef RUN_SYSTEM_INFO_DLG_IN_SEPARATE_THREAD
//	CDialog::OnClose();
//#else
//	DestroyWindow();
//#endif
}

void CSystemInfoDlg::OnOK()
{
	if( IsInitFind() )
	{
	   RefreshList();
	}
}

void CSystemInfoDlg::OnCancel()
{
	TRACE( _T("TaskManagerEx: CSystemInfoDlg::OnCancel()\n") );

	// BUG! This code is not called when RUN_SYSTEM_INFO_DLG_IN_SEPARATE_THREAD is undefined && "Esc" button is pressed!
	// I don't know why...

#ifdef RUN_SYSTEM_INFO_DLG_IN_SEPARATE_THREAD
	EndDialog( IDCANCEL );
#else
	DestroyWindow();
#endif
}

void CSystemInfoDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	//TRACE( _T("TaskManagerEx: CSystemInfoDlg::OnKeyDown()\n") );
	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSystemInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSystemInfoDlg)
	DDX_Control(pDX, IDC_LISTVIEW, m_SystemInfoList);
	//}}AFX_DATA_MAP
}

#define WM_TMEX_LIST_UPDATE		WM_USER + 1

#define ON_WM_TMEX_LIST_UPDATE() \
	{ WM_TMEX_LIST_UPDATE, 0, 0, 0, AfxSig_vv, \
		(AFX_PMSG)(AFX_PMSGW)(void (AFX_MSG_CALL CWnd::*)(void))&OnListUpdate },


BEGIN_MESSAGE_MAP(CSystemInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CSystemInfoDlg)
	ON_WM_SIZE()
	ON_COMMAND(ID_EXTENSION_REFRESH, OnExtensionRefresh)
	ON_WM_CONTEXTMENU()
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_HANDLES_RELEASE, OnHandlesRelease)
	ON_COMMAND(ID_MODULES_UNLOAD, OnModulesUnload)
	ON_COMMAND(ID_WINDOWS_CLOSE, OnWindowsClose)
	ON_COMMAND(ID_HANDLES_EVENT_SET, OnHandlesEventSet)
	ON_COMMAND(ID_HANDLES_EVENT_RESET, OnHandlesEventReset)
	ON_COMMAND(ID_HANDLES_EVENT_PULSE, OnHandlesEventPulse)
	ON_COMMAND(ID_HANDLES_MUTANT_RELEASE, OnHandlesMutantRelease)
	ON_COMMAND(ID_MODULES_LOAD, OnModulesLoad)
	ON_COMMAND(ID_HANDLES_PROCESS_EXITPROCESS, OnHandlesProcessExitprocess)
	ON_COMMAND(ID_HANDLES_PROCESS_TERMINATE, OnHandlesProcessTerminate)
	ON_COMMAND(ID_HANDLES_THREAD_RESUME, OnHandlesThreadResume)
	ON_COMMAND(ID_HANDLES_THREAD_SUSPEND, OnHandlesThreadSuspend)
	ON_COMMAND(ID_HANDLES_THREAD_TERMINATE, OnHandlesThreadTerminate)
	ON_COMMAND(ID_HANDLES_WINDOWSTATION_ASSIGNTOPROCESS, OnHandlesWindowstationAssigntoprocess)
	ON_COMMAND(ID_HANDLES_DESKTOP_SWITHTO, OnHandlesDesktopSwithto)
	ON_COMMAND(ID_MODULES_FILEPROPERTIES, OnModulesFileproperties)
	ON_COMMAND(ID_MODULES_DEPENDENCY, OnModulesDependency)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTVIEW, OnItemchangedListview)
	ON_COMMAND(ID_FIND_TEXT, OnFindText)
	ON_COMMAND(ID_FIND_NEXT_TEXT, OnFindNextText)
	ON_COMMAND(ID_FIND_PREV_TEXT, OnFindPrevText)
	ON_COMMAND(ID_MEMORY_EXPAND, OnMemoryExpand)
	ON_COMMAND(ID_MEMORY_COMPACT, OnMemoryCompact)
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
	ON_WM_PAINT()
	ON_COMMAND(ID_HANDLES_SECURITY, OnHandlesSecurity)
	ON_COMMAND(ID_MODULES_SECURITY, OnModulesSecurity)
	ON_COMMAND(ID_HANDLES_THREAD_SECURITY, OnHandlesThreadSecurity)
	ON_WM_CLOSE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNotify )
	ON_WM_TMEX_LIST_UPDATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemInfoDlg message handlers

BOOL CSystemInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon( theApp.hMainIconBig, TRUE );
	SetIcon( theApp.hMainIconSmall, FALSE );
//	SetClassLong( this->GetSafeHwnd(), GCL_HICON, (long)theApp.hMainIconBig );		// this changes icon for any dialog in the process!
//	SetClassLong( this->GetSafeHwnd(), GCL_HICONSM, (long)theApp.hMainIconSmall );	// this changes icon for any dialog in the process!

	CreateToolbar();
	CreateStatusBar();

	Resize();

	bool bAlwaysOnTop = theApp.IsTaskManagerWindowTopMost();
	if( bAlwaysOnTop )
	{
		::SetWindowPos( *this, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
	}

	m_ImageList.Create( LIST_IMAGE_WIDTH, LIST_IMAGE_WIDTH, ILC_COLOR, 0, 1 );
	if( GetInitCommand() == ID_PROCESSES_MEMORY )
	{
		m_SystemInfoList.SetImageList( &m_ImageList, LVSIL_SMALL );
	}

	RefreshAll();
//	ResizeToListCtrl();

//	this->EnableToolTips( TRUE );

	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.Activate(TRUE);

//		m_wndToolBar.EnableToolTips( TRUE );
//		m_wndToolBar.GetToolBarCtrl().SetToolTips( &m_tooltip );

		m_tooltip.AddTool( &m_wndToolBar, LPSTR_TEXTCALLBACK);

		//m_tooltip.AddTool( CWnd::FromHandle(m_SystemInfoList.GetHeaderCtrl()->GetSafeHwnd()), LPSTR_TEXTCALLBACK);
		//m_tooltip.AddTool( &m_SystemInfoList, LPSTR_TEXTCALLBACK);

		//m_tooltip.AddTool( &m_wndStatusBar, LPSTR_TEXTCALLBACK);

		//m_tooltip.AddTool( &m_SystemInfoList, IDC_LISTVIEW);
		//m_SystemInfoList.EnableToolTips();
		//m_SystemInfoList.EnableTrackingToolTips();

		if( IsInitFind() )
		{
			UINT uTooltip = 0;
			switch( GetInitCommand() )
			{
			case ID_EXTENSION_FINDUSEDFILE:		uTooltip = IDS_FIND_FILE_EDIT; break;
			case ID_EXTENSION_FINDUSEDMODULE:	uTooltip = IDS_FIND_MODULE_EDIT; break;
			default:							ASSERT(FALSE); break;
			}
			m_tooltip.AddTool( &m_fileNameCtrl, LocLoadString(uTooltip) );

			//m_tooltip.AddTool( &m_fileNameStaticCtrl, _T("sdfSADDSA") ); // don't work!
		}

		//m_wndStatusBar.EnableToolTips();
		//m_wndStatusBar.EnableTrackingToolTips(); // ???
		//m_wndStatusBar.GetStatusBarCtrl().SetTipText( 0, _T("Status") );
		//m_wndStatusBar.GetStatusBarCtrl().SetTipText( 1, _T("Item count") );

		// TODO: Use one of the following forms to add controls:
		// m_tooltip.AddTool(GetDlgItem(IDC_<name>), <string-table-id>);
		// m_tooltip.AddTool(GetDlgItem(IDC_<name>), "<text>");
	}

	{
		CFindTextDlg m_FindDlg;
		m_FindOptions = m_FindDlg.m_options;
	}

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSystemInfoDlg::OnDestroy() 
{
	TRACE( _T("TaskManagerEx: CSystemInfoDlg::OnDestroy()\n") );
	CDialog::OnDestroy();
	
	m_ImageList.DeleteImageList();
}

INT_PTR CSystemInfoDlg::DoModal()
{
	// load resource as necessary
	if( m_lpszTemplateName != NULL && m_lpDialogTemplate == NULL )
	{
		m_lpDialogTemplate = LocLoadDialog( m_lpszTemplateName );
		m_lpszTemplateName = NULL;
	}

	return CDialog::DoModal();
}

BOOL CSystemInfoDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	pHelpInfo = pHelpInfo;
	CAboutExtensionThread::Start( this );
	return TRUE;
	//return CDialog::OnHelpInfo(pHelpInfo);
}

BOOL CSystemInfoDlg::OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	id = id;
	*pResult = 0;

	NMHDR* pNMHDR = pTTTStruct;
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
    UINT_PTR nID = pNMHDR->idFrom;

	//TRACE( _T("OnToolTipNotify> id = %d, from = %d (0x%X) \n"), id, nID, nID );

	//TRACE( _T("OnToolTipNotify> m_fileNameStaticCtrl: 0x%X\n"), m_fileNameStaticCtrl.GetSafeHwnd() );
	//TRACE( _T("OnToolTipNotify> m_fileNameCtrl: 0x%X\n"), m_fileNameCtrl.GetSafeHwnd() );
	//TRACE( _T("OnToolTipNotify> m_wndToolBar: 0x%X\n"), m_wndToolBar.GetSafeHwnd() );
	//TRACE( _T("OnToolTipNotify> m_wndStatusBar: 0x%X\n"), m_wndStatusBar.GetSafeHwnd() );
	//TRACE( _T("OnToolTipNotify> m_SystemInfoList: 0x%X\n"), m_SystemInfoList.GetSafeHwnd() );

	if (pTTT->uFlags & TTF_IDISHWND)
    {
		BOOL bChanged = FALSE;
		//TRACE( _T("OnToolTipNotify> Window\n") );

		CPoint pt(0,0);
		GetCursorPos( &pt );

		if( IsInitFind() )
		{
			bChanged = TRUE;
			CRect rect(0,0,0,0);
			m_fileNameStaticCtrl.GetWindowRect( &rect );
			BOOL bStaticText = rect.PtInRect( pt );
			if( bStaticText ) //nID == (UINT)m_fileNameStaticCtrl.GetSafeHwnd() )
			{
				UINT uTooltip = 0;
				switch( GetInitCommand() )
				{
				case ID_EXTENSION_FINDUSEDFILE:		uTooltip = IDS_FIND_FILE_STATIC; break;
				case ID_EXTENSION_FINDUSEDMODULE:	uTooltip = IDS_FIND_MODULE_STATIC; break;
				default:							ASSERT(FALSE); break;
				}
				nID = uTooltip;
			}
		}
/*
		CHeaderCtrl* pHeader = m_SystemInfoList.GetHeaderCtrl();
		if( pHeader != NULL && nID == (UINT) pHeader->GetSafeHwnd() )
		{
			//TRACE( _T("OnToolTipNotify> GetHeaderCtrl\n") );
			int n = pHeader->GetItemCount();
			for( int i=0; i<n; i++ )
			{
				CRect r(0,0,0,0);
				pHeader->GetItemRect( i, &r );
				if( r.PtInRect( pt ) ) 
				{
					TCHAR szText[200] = _T("");
					HDITEM item;
					item.mask = HDI_TEXT;
					item.cchTextMax = SIZEOF_ARRAY(szText);
					item.pszText = szText;
					pHeader->GetItem( i, &item );
					_tcsncpy( pTTT->szText, szText, SIZEOF_ARRAY(pTTT->szText) );
					pTTT->lpszText = pTTT->szText;
					pTTT->hinst = NULL;
					return(TRUE);
				}
			}
		}

		if( nID == (UINT) m_wndStatusBar.GetSafeHwnd() )
		{
			// TODO: Fill status bar's panes tooltips
		}

		if( nID == (UINT) m_SystemInfoList.GetSafeHwnd() )
		{
			//TRACE( _T("OnToolTipNotify> m_SystemInfoList\n") );
			//UINT uFlags = 0;
			//int nItem = m_SystemInfoList.HitTest( pt, &uFlags );
			LVHITTESTINFO lvhti;
			lvhti.pt = pt;
			m_SystemInfoList.ScreenToClient( &lvhti.pt );
			m_SystemInfoList.SubItemHitTest(&lvhti);
			CString s = m_SystemInfoList.GetItemText( lvhti.iItem, lvhti.iSubItem );
			s = s.Left( SIZEOF_ARRAY(pTTT->szText)-1 );
			//TRACE( _T("SubItem text: \"%s\"\n"), s );
			_tcsncpy( pTTT->szText, s, SIZEOF_ARRAY(pTTT->szText) );
			pTTT->lpszText = pTTT->szText;
			pTTT->hinst = NULL;
			return(TRUE);
		}
*/
		if( !bChanged )
		{
			// idFrom is actually the HWND of the tool
			nID = ::GetDlgCtrlID((HWND)nID);
		}
	}

	//CToolBarCtrl& toolbar = m_wndToolBar.GetToolBarCtrl();
	//int iHotItem = toolbar.GetHotItem();
	//TRACE( _T("iHotItem = %d\n"), iHotItem );
	//TBBUTTON info;
	//toolbar.GetButton( iHotItem, &info );
	//nID = info.idCommand;

	if(nID != 0)
	{
		CString s;
		s.LoadString(nID);
		if( nID < 41000 )
		{
			s = LocLoadString(nID);
		}
		TRACE(_T("Load string #%Id = \"%s\"\n"), nID, s);
		s = s.Left( SIZEOF_ARRAY(pTTT->szText)-1 );
		_tcsncpy( pTTT->szText, s, SIZEOF_ARRAY(pTTT->szText) );
		pTTT->lpszText = pTTT->szText;
		pTTT->hinst = NULL;
		return(TRUE);
	}

	return(FALSE);
}

BOOL CSystemInfoDlg::CreateToolbar()
{
	BOOL res;
	CRect rcClientStart(0,0,0,0);
	GetClientRect(rcClientStart);

	UINT toolbarID = IDR_SYSTEMINFODLG;

	if ( !m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
			| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
			!m_wndToolBar.LoadToolBar(toolbarID))
		return TRUE;

	if( IsInitFind() )
	{
		CRect rect;
		CString staticText;
		switch( GetInitCommand() )
		{
		case ID_EXTENSION_FINDUSEDFILE:		staticText = LocLoadString(IDS_LIST_FIND_FILE_NAME); break;
		case ID_EXTENSION_FINDUSEDMODULE:	staticText = LocLoadString(IDS_LIST_FIND_MODULE_NAME); break;
		default:			ASSERT(FALSE);	staticText = LocLoadString(IDS_LIST_FIND_UNKNOWN_NAME); break;
		}

		m_wndToolBar.SetButtonInfo( 0, 0, TBBS_SEPARATOR, GetDC()->GetTextExtent( staticText ).cx );
		m_wndToolBar.SetButtonInfo( 1, 0, TBBS_SEPARATOR, 120);

		m_wndToolBar.GetItemRect( 0, &rect);
		rect.top += 2;
		rect.left += 5;

		res = m_fileNameStaticCtrl.Create( staticText, WS_CHILD | WS_VISIBLE,
			rect, &m_wndToolBar, ID_TOOLBAR_BUTTON1 ); // ID_TOOLBAR_BUTTON1 - for example, may be any, even 0 (zero)
		if( !res )
			return FALSE;

		m_wndToolBar.GetItemRect( 1, &rect);

		res = m_fileNameCtrl.Create( WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, rect,
			&m_wndToolBar, ID_TOOLBAR_BUTTON2 ); // ID_TOOLBAR_BUTTON2 - for example, may be any
		if( !res )
			return FALSE;

		m_fileNameCtrl.ModifyStyleEx( 0, WS_EX_CLIENTEDGE );

		m_fileNameStaticCtrl.SetFont( GetFont() );
		m_fileNameCtrl.SetFont( GetFont() );

		m_fileNameStaticCtrl.ShowWindow(SW_SHOW);
		m_fileNameCtrl.ShowWindow(SW_SHOW);

		m_fileNameCtrl.SetFocus();
	}

	CToolBarCtrl& toolbar = m_wndToolBar.GetToolBarCtrl();

	if( GetInitCommand() != ID_PROCESSES_MEMORY )
	{
		int iIndex;
		iIndex = toolbar.CommandToIndex( ID_MEMORY_EXPAND );
		toolbar.DeleteButton( iIndex );
		iIndex = toolbar.CommandToIndex( ID_MEMORY_COMPACT );
		toolbar.DeleteButton( iIndex );
	}

	if( !IsInitFind() )
	{
		int iIndex;
		iIndex = toolbar.CommandToIndex( ID_TOOLBAR_BUTTON1 );
		toolbar.DeleteButton( iIndex );
		iIndex = toolbar.CommandToIndex( ID_TOOLBAR_BUTTON2 );
		toolbar.DeleteButton( iIndex );
	}

	{
		UINT nID;
		UINT nStyle;
		int iImage;
		int iIndex;

		if( !IsInitFind() )
		{
			iIndex = 0;
			nID = 0;
			nStyle = 0;
			iImage = 0;
			m_wndToolBar.GetButtonInfo( iIndex, nID, nStyle, iImage );
			if( (nStyle & TBSTYLE_SEP) != 0 )
			{
				toolbar.DeleteButton( iIndex );
			}
		}

		{
			iIndex = toolbar.GetButtonCount() - 1;
			nID = 0;
			nStyle = 0;
			iImage = 0;
			m_wndToolBar.GetButtonInfo( iIndex, nID, nStyle, iImage );
			if( (nStyle & TBSTYLE_SEP) != 0 )
			{
				toolbar.DeleteButton( iIndex );
			}
		}
	}

	// We need to resize the dialog to make room for control bars.
	// First, figure out how big the control bars are.
	CRect rcClientNow(0,0,0,0);
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
	MoveWindow(rcWindow, TRUE); // It will repaint later, because initialization process is in progress!

	// And position the control bars
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	return TRUE;
}

BOOL CSystemInfoDlg::CreateStatusBar()
{
	m_wndStatusBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM | CBRS_TOOLTIPS );

	m_wndStatusBar.SetIndicators(indicators,2);

	CRect rect;
	GetClientRect(&rect);
	m_wndStatusBar.SetPaneInfo( 0, ID_INDICATOR_PANE1, SBPS_STRETCH, 0 );
	m_wndStatusBar.SetPaneInfo( 1, ID_INDICATOR_PANE2, SBPS_NORMAL,  150 );

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,
		0 );
//		ID_INDICATOR_PANE1); // POSSIBLE BUG?

	m_wndStatusBar.SetPaneText(0, LocLoadString(IDS_LIST_STATUS_READY) );
	m_wndStatusBar.SetPaneText(1, _T("") );

	return TRUE;
}

void CSystemInfoDlg::RefreshAll()
{
	int nColumnCount = m_SystemInfoList.GetHeaderCtrl()->GetItemCount();

	// Delete all of the columns.
	for (int i=0;i < nColumnCount;i++)
	{
		m_SystemInfoList.DeleteColumn(0);
	}

	CString strCaption;

	switch ( GetInitCommand() )
	{
	case ID_PROCESSES_FILES:
	case ID_PROCESSES_MODULES:
	case ID_PROCESSES_WINDOWS:
	case ID_PROCESSES_HANDLES:
	case ID_PROCESSES_THREADS:
	case ID_PROCESSES_MEMORY:
		{
			TCHAR szFileName[MAX_PATH] = _T("");
			GetProcessExecutableName( GetInitProcessId(), szFileName, SIZEOF_ARRAY(szFileName) );
			CString s = szFileName;
			int pos = s.ReverseFind( _T('\\') );

			strCaption.Format( _T("PID = %d(0x%X) - %s - "),
				GetInitProcessId(), GetInitProcessId(), s.Mid(pos+1) );
		}
		break;

	case ID_EXTENSION_FINDUSEDFILE:
		strCaption = LocLoadString(IDS_LIST_CAPTION_FIND_USED_FILE);
		break;

	case ID_EXTENSION_FINDUSEDMODULE:
		strCaption = LocLoadString(IDS_LIST_CAPTION_FIND_USED_MODULE);
		break;

	case ID_EXTENSION_ALL_HANDLES:
		strCaption = LocLoadString(IDS_LIST_CAPTION_ALL_HANDLES);
		break;

	case ID_EXTENSION_SHOW_DRIVERS:
		if( GetInitProcessId() == 0 )
		{
			strCaption = LocLoadString(IDS_LIST_CAPTION_LOADED_KERNEL_MODULES);
		}
		else
		{
			CString strProcessInfo = LocLoadString(IDS_LIST_CAPTION_KERNEL_PROCESS);

			strCaption.Format( _T("PID = %d(0x%X) - %s - %s"),
				GetInitProcessId(), GetInitProcessId(), strProcessInfo,
				LocLoadString(IDS_LIST_CAPTION_KERNEL_MODULES) );
		}
		break;

	default:
		ASSERT(FALSE);
	}

	// Columns
	switch ( GetInitCommand() )
	{
	case ID_PROCESSES_FILES:
		strCaption += LocLoadString(IDS_LIST_CAPTION_SUB_FILES);
		SystemHandleInformation::HANDLE_INFORMATION::InsertFileColumns( m_SystemInfoList, FALSE );
		break;

	case ID_PROCESSES_MODULES:
		strCaption += LocLoadString(IDS_LIST_CAPTION_SUB_MODULES);
		SystemModuleInformation::MODULE_INFO::InsertColumns( m_SystemInfoList, FALSE );
		break;

	case ID_PROCESSES_WINDOWS:
		strCaption += LocLoadString(IDS_LIST_CAPTION_SUB_WINDOWS);

		m_SystemInfoList.InsertColumn( 0, COLUMN_HANDLE );
		m_SystemInfoList.InsertColumn( 1, COLUMN_WINDOW_CAPTION );
		m_SystemInfoList.InsertColumn( 2, COLUMN_WINDOW_CLASS );
		m_SystemInfoList.InsertColumn( 3, COLUMN_WINDOW_RECT );
		m_SystemInfoList.InsertColumn( 4, COLUMN_WINDOW_STYLE );
		m_SystemInfoList.InsertColumn( 5, COLUMN_WINDOW_EX_STYLE );

		break;

	case ID_PROCESSES_HANDLES:
		strCaption += LocLoadString(IDS_LIST_CAPTION_SUB_HANDLES);
		SystemHandleInformation::HANDLE_INFORMATION::InsertColumns( m_SystemInfoList, FALSE );
		break;

	case ID_PROCESSES_THREADS:
		strCaption += LocLoadString(IDS_LIST_CAPTION_SUB_THREADS);
		SystemThreadInformation::THREAD_INFORMATION::InsertColumns( m_SystemInfoList, FALSE );
		break;

	case ID_PROCESSES_MEMORY:
		strCaption += LocLoadString(IDS_LIST_CAPTION_SUB_MEMORY_MAP);
		SystemMemoryMapInformation::MEMORY_INFORMATION::InsertColumns( m_SystemInfoList );
		break;

	case ID_EXTENSION_FINDUSEDFILE:
		SystemHandleInformation::HANDLE_INFORMATION::InsertFileColumns( m_SystemInfoList, TRUE );
		break;

	case ID_EXTENSION_FINDUSEDMODULE:
		SystemModuleInformation::MODULE_INFO::InsertColumns( m_SystemInfoList, TRUE );
		break;

	case ID_EXTENSION_ALL_HANDLES:
		SystemHandleInformation::HANDLE_INFORMATION::InsertColumns( m_SystemInfoList, TRUE );
		break;

	case ID_EXTENSION_SHOW_DRIVERS:
		SystemKernelModuleInformation::KERNEL_MODULE_INFORMATION::InsertColumns( m_SystemInfoList );
		break;
	}

	SetWindowText( strCaption );

	CenterWindow();
	ShowWindow( SW_SHOW );
	UpdateWindow();			// for long queries like "Show all handles"
//	RedrawWindow();
//	PostMessage( WM_PAINT, 0, 0 );

	
	if( !IsInitFind() )
	{
		PostMessage( WM_TMEX_LIST_UPDATE, 0, 0 );
	}
//	RefreshList();
}

void CSystemInfoDlg::OnListUpdate()
{
	//TRACE( _T("CSystemInfoDlg::OnListUpdate()\n") );
	m_bResizeListDuringRefresh = TRUE;
	RefreshList();
	m_bResizeListDuringRefresh = FALSE;
}

void CSystemInfoDlg::RefreshList()
{
	if( !IsInitMultiprocess() )
	{
		if ( !theApp.IsProcessIdValid( GetInitProcessId() ) )
		{
			//m_SystemInfoList.DeleteAllItems();
			int res = AfxMessageBox( LocLoadString(IDS_PROCESS_IS_NOT_VALID), MB_YESNO );
			if ( res == IDYES )
			{
				EndDialog(0);
			}
			return;
		}
	}

	BeginWaitCursor();
	m_wndStatusBar.SetPaneText( 0, LocLoadString(IDS_LIST_STATUS_BUSY) );
	//m_wndStatusBar.RedrawWindow();

	SCROLLINFO vsi = {0};
	vsi.cbSize = sizeof(vsi);
	m_SystemInfoList.GetScrollInfo( SB_VERT, &vsi );
	SCROLLINFO hsi = {0};
	hsi.cbSize = sizeof(hsi);
	m_SystemInfoList.GetScrollInfo( SB_HORZ, &hsi );

	int iTopIndex = m_SystemInfoList.GetTopIndex();
	int iItemSel = m_SystemInfoList.GetSelectedItem();

	//TRACE( _T("CSystemInfoDlg::RefreshList()> item %d, scroll: %d (%d - %d), page = %d, top = %d\n"),
	//	iItemSel, vsi.nPos, vsi.nMin, vsi.nMax, vsi.nPage, iTopIndex );
	//TRACE( _T("CSystemInfoDlg::RefreshList()> horizont:scroll: %d (%d - %d), page = %d\n"),
	//	hsi.nPos, hsi.nMin, hsi.nMax, hsi.nPage );

	m_SystemInfoList.SetRedraw( FALSE );

	m_SystemInfoList.DeleteAllItems();
	BOOL bFindOK = FALSE;

	// List items
	switch ( GetInitCommand() )
	{
	case ID_PROCESSES_FILES:
		{
			SystemHandleInformation hi( GetInitProcessId(), TRUE, _T("File") );

			int iItem = 0;
			int iItemCount = hi.m_HandleInfos.GetCount();
			for ( POSITION pos = hi.m_HandleInfos.GetHeadPosition(); pos != NULL; )
			{
				SystemHandleInformation::HANDLE_INFORMATION& h = hi.m_HandleInfos.GetNext(pos);

				CString strDevice, strPath;
				SystemHandleInformation::GetName( (HANDLE)h.sh.HandleNumber, strDevice, h.sh.ProcessID );
				SystemInfoUtils::GetFsFileName( strDevice, strPath );

				h.InsertFile( m_SystemInfoList, FALSE, iItem, iItemCount, strDevice, strPath );
				iItem++;
			}
		}
		break;

	case ID_PROCESSES_MODULES:
		{
			SystemModuleInformation mi( GetInitProcessId(), TRUE );

			int iItem = 0;
			int iItemCount = mi.m_ModuleInfos.GetCount();
			for ( POSITION pos = mi.m_ModuleInfos.GetHeadPosition(); pos != NULL; )
			{
				SystemModuleInformation::MODULE_INFO& m = mi.m_ModuleInfos.GetNext(pos);

				m.Insert( m_SystemInfoList, FALSE, iItem, iItemCount );
				iItem++;
			}
		}
		break;

	case ID_PROCESSES_WINDOWS:
		{
			SystemWindowInformation wi( GetInitProcessId(), TRUE );

			for ( POSITION pos = wi.m_WindowInfos.GetHeadPosition(); pos != NULL; )
			{
				TCHAR szBuffer[0x100];
				CString strItem;
				SystemWindowInformation::WINDOW_INFO& w = wi.m_WindowInfos.GetNext(pos);

				strItem.Format( _T("0x%08X"), w.hWnd );

				int iItemCount = m_SystemInfoList.GetItemCount();
				int nPos = m_SystemInfoList.InsertItem( iItemCount, strItem );

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
			SystemHandleInformation hi( GetInitProcessId(), TRUE, NULL );

			int iItem = 0;
			int iItemCount = hi.m_HandleInfos.GetCount();
			for ( POSITION pos = hi.m_HandleInfos.GetHeadPosition(); pos != NULL; )
			{
				SystemHandleInformation::HANDLE_INFORMATION& h = hi.m_HandleInfos.GetNext(pos);

				h.Insert( m_SystemInfoList, FALSE, iItem, iItemCount );
				iItem++;
			}
		}
		break;

	case ID_PROCESSES_THREADS:
		{
			SystemThreadInformation ti( GetInitProcessId(), TRUE );

			int iItem = 0;
			int iItemCount = ti.m_ThreadInfos.GetCount();
			for ( POSITION pos = ti.m_ThreadInfos.GetHeadPosition(); pos != NULL; )
			{
				SystemThreadInformation::THREAD_INFORMATION& t = ti.m_ThreadInfos.GetNext(pos);

				t.Insert( m_SystemInfoList, FALSE, iItem, iItemCount );
				iItem++;
			}
		}
		break;

	case ID_PROCESSES_MEMORY:
		{
			SystemMemoryMapInformation smi( GetInitProcessId(), m_bExpandRegions, TRUE );

			int iItem = 0;
			int iItemCount = smi.m_MemoryInfos.GetCount();
			//TRACE( _T("Got %d memory blocks\n"), smi.m_MemoryInfos.GetCount() );
			for ( POSITION pos = smi.m_MemoryInfos.GetHeadPosition(); pos != NULL; )
			{
				SystemMemoryMapInformation::MEMORY_INFORMATION& mi = smi.m_MemoryInfos.GetNext(pos);

				mi.Insert( m_SystemInfoList, iItem, iItemCount, m_bExpandRegions );
				iItem++;
			}
		}
		break;

	case ID_EXTENSION_FINDUSEDFILE:
		{
			BOOL bAll;
			CString findFileName;
			m_fileNameCtrl.GetWindowText( findFileName );

			if( findFileName == _T("") )
			{
				findFileName = _T("*");
				//break;
			}

			bAll = findFileName == _T("*");

			SystemHandleInformation hi( ALL_PROCESSES, TRUE, _T("File") );

			int iItem = 0;
			int iItemCount = hi.m_HandleInfos.GetCount();
			for( POSITION pos = hi.m_HandleInfos.GetHeadPosition(); pos != NULL; )
			{
				SystemHandleInformation::HANDLE_INFORMATION& h = hi.m_HandleInfos.GetNext(pos);

				CString strDevice, strPath;
				SystemHandleInformation::GetName( (HANDLE)h.sh.HandleNumber, strDevice, h.sh.ProcessID );
				SystemInfoUtils::GetFsFileName( strDevice, strPath );


				if( !bAll )
				{
					BOOL bFoundInDevice = FreeSearchInString( strDevice, findFileName );
					BOOL bFoundInFileName = FreeSearchInString( strPath, findFileName );
					if( !bFoundInDevice && !bFoundInFileName )
					{
						continue;
					}
				}

				h.InsertFile( m_SystemInfoList, TRUE, iItem, iItemCount, strDevice, strPath );
				iItem++;
			}

			bFindOK = m_SystemInfoList.GetItemCount() > 0;
		}
		break;

	case ID_EXTENSION_FINDUSEDMODULE:
		{
			BOOL bAll;
			CString findFileName;
			m_fileNameCtrl.GetWindowText( findFileName );

			if( findFileName == _T("") )
			{
				findFileName = _T("*");
				//break;
			}

			bAll = findFileName == _T("*");

			SystemModuleInformation mi( ALL_PROCESSES, TRUE );

			int iItem = 0;
			int iItemCount = mi.m_ModuleInfos.GetCount();
			for( POSITION pos = mi.m_ModuleInfos.GetHeadPosition(); pos != NULL; )
			{
				SystemModuleInformation::MODULE_INFO& m = mi.m_ModuleInfos.GetNext(pos);

				if( !bAll )
				{
					BOOL bFoundInFileName = FreeSearchInString( m.FullPath, findFileName );
					if( !bFoundInFileName )
					{
						continue;
					}
				}

				m.Insert( m_SystemInfoList, TRUE, iItem, iItemCount );
				iItem++;
			}

			bFindOK = m_SystemInfoList.GetItemCount() > 0;
		}
		break;

	case ID_EXTENSION_ALL_HANDLES:
		{
			SystemHandleInformation hi( ALL_PROCESSES, TRUE, NULL );

			int iItem = 0;
			int iItemCount = hi.m_HandleInfos.GetCount();
			for ( POSITION pos = hi.m_HandleInfos.GetHeadPosition(); pos != NULL; )
			{
				SystemHandleInformation::HANDLE_INFORMATION& h = hi.m_HandleInfos.GetNext(pos);

				h.Insert( m_SystemInfoList, TRUE, iItem, iItemCount );
				iItem++;
			}
		}
		break;

	case ID_EXTENSION_SHOW_DRIVERS:
		{
			SystemKernelModuleInformation kmi( TRUE );

			int iItem = 0;
			int iItemCount = kmi.m_KernelModuleInfos.GetCount();
			for ( POSITION pos = kmi.m_KernelModuleInfos.GetHeadPosition(); pos != NULL; )
			{
				SystemKernelModuleInformation::KERNEL_MODULE_INFORMATION& km = kmi.m_KernelModuleInfos.GetNext(pos);

				km.Insert( m_SystemInfoList, iItem, iItemCount );
				iItem++;
			}
		}
		break;
	};

	ResizeColumns();
	m_SystemInfoList.ReSort();

	CRect r(0,0,0,0);
	m_SystemInfoList.GetItemRect( iTopIndex-1, &r, LVIR_BOUNDS );
	m_SystemInfoList.Scroll( CSize(hsi.nPos, r.top) );

	if( iItemSel != -1 )
	{
	   m_SystemInfoList.SetSelectedItem( iItemSel );
	   m_SystemInfoList.EnsureVisible( iItemSel, TRUE );
	}

	m_SystemInfoList.SetRedraw( TRUE );

	// invalidate the entire control, force painting
	m_SystemInfoList.Invalidate();		// strange results
	m_SystemInfoList.UpdateWindow();	// strange results
//	m_SystemInfoList.RedrawWindow();	// strange results

	RefreshStatus();

	if( m_bResizeListDuringRefresh )
	{
		//TRACE( _T("ResizeToListCtrl because of m_bResizeListDuringRefresh\n") );
		ResizeToListCtrl();
	}
	else if( bFindOK && m_bFirstFind )
	{
		//TRACE( _T("ResizeToListCtrl because of m_bFirstFind\n") );
		m_bFirstFind = FALSE;
		ResizeToListCtrl();
	}

	if( IsInitFind() )
	{
	   m_fileNameCtrl.SetSel( 0, -1 ); // select all
	   m_fileNameCtrl.SetFocus();
	}

	m_wndStatusBar.SetPaneText( 0, LocLoadString(IDS_LIST_STATUS_READY) );
	EndWaitCursor();

	return;
}

void CSystemInfoDlg::RefreshStatus()
{
	int iItems = m_SystemInfoList.GetItemCount();
	int iSelected = m_SystemInfoList.GetSelectedCount();
	CString s;
	s.Format( LocLoadString(IDS_LIST_STATUS_ITEMS), iItems, iSelected );
	m_wndStatusBar.SetPaneText(1, s );
}

void CSystemInfoDlg::Resize()
{
	CRect rect (0,0,0,0);
	CRect toolbarRect (0,0,0,0);
	CRect statusbarRect (0,0,0,0);

	GetClientRect( &rect );

	if ( m_wndToolBar.GetSafeHwnd() != NULL )
	{
		m_wndToolBar.GetWindowRect( &toolbarRect );

		if ( IsInitFind() )
		{
			CRect rect1;
			CRect rect3;
			CRect rectLast;
			int w;
			int h;

			int iButtons = m_wndToolBar.GetToolBarCtrl().GetButtonCount();
			m_wndToolBar.GetItemRect( 0, &rect1 );
			m_wndToolBar.GetItemRect( 2, &rect3 );
			m_wndToolBar.GetItemRect( iButtons-1, &rectLast );

			w = rect.Width() - rect1.Width() -
				( rectLast.right - rect3.left );
				;
			h = toolbarRect.Height() - 2;

			m_wndToolBar.SetButtonInfo( 1, 0, TBBS_SEPARATOR, w );
			m_fileNameCtrl.SetWindowPos( NULL, 0, 0, w, h, SWP_NOMOVE );
		}
	}

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	if ( m_wndToolBar.GetSafeHwnd() != NULL )
	{
		m_wndToolBar.GetWindowRect( &toolbarRect );
		ScreenToClient( &toolbarRect );
	}
	if ( m_wndStatusBar.GetSafeHwnd() != NULL )
	{
		m_wndStatusBar.GetWindowRect( &statusbarRect );
		ScreenToClient( &statusbarRect );
	}

	if ( m_SystemInfoList.GetSafeHwnd() != NULL )
	{
		m_SystemInfoList.MoveWindow( 0, toolbarRect.Height(),
			rect.Width(), rect.Height() - toolbarRect.Height() - statusbarRect.Height(), TRUE );
	}
}

void CSystemInfoDlg::ResizeColumns()
{
	int colCount = m_SystemInfoList.GetHeaderCtrl()->GetItemCount();
	int rowCount = m_SystemInfoList.GetItemCount();
	int iMinimalWidth = rowCount > 0 ? MINIMAL_COLUMN_WIDTH : DEFAULT_COLUMN_WIDTH;

	for ( int col = 0; col < colCount; col++)
	{
	  m_SystemInfoList.SetColumnWidth( col, LVSCW_AUTOSIZE );

	  m_SystemInfoList.SetColumnWidth( col, max( iMinimalWidth, m_SystemInfoList.GetColumnWidth( col ) ) );
	}
}

void CSystemInfoDlg::ResizeToListCtrl()
{
//	TRACE( _T("CSystemInfoDlg::ResizeToListCtrl()\n") );
	CRect rectWindow;
	CRect rectDesktop;

	GetWindowRect( &rectWindow );
	int DesiredWidth = 30;
	int height = rectWindow.Height();

	int screenWidth = 800;
	BOOL res = SystemParametersInfo( SPI_GETWORKAREA, 0, &rectDesktop, 0 );
	if( res )
	{
		screenWidth = rectDesktop.Width();
	}

	int colCount = m_SystemInfoList.GetHeaderCtrl()->GetItemCount();

	for ( int col = 0; col < colCount; col++)
		DesiredWidth += m_SystemInfoList.GetColumnWidth( col );

	int iMaxWidth = screenWidth*9/10;
	int iNewWidth = max( 350, min( DesiredWidth, iMaxWidth ) );

	MoveWindow( rectWindow.left - (iNewWidth-rectWindow.Width())/2, rectWindow.top,
		iNewWidth, height, TRUE );
//	RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME );
//	Invalidate();
}

void CSystemInfoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if( !IsIconic() )
	{
		Resize();
	}
}

void CSystemInfoDlg::OnExtensionRefresh()
{
	//TRACE( _T("OnExtensionRefresh()\n") );
	RefreshList();
}

BOOL CSystemInfoDlg::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}

	BOOL bTranslated =  TranslateAccelerator( this->GetSafeHwnd(), m_hAccelTable, pMsg);
	if( bTranslated )
	{
		//TRACE( _T("Accelerator Translated!\n") );
		return TRUE; // translated and should not be dispatched
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CSystemInfoDlg::OnContextMenu(CWnd* pWnd, CPoint )
{
	if ( pWnd != &m_SystemInfoList )
	{
		return;
	}

	int iSelected = m_SystemInfoList.GetSelectedCount();

	if( iSelected != 1 )
	{
		return;
	}

	CMenu popupMenu;
	CMenu* pSubMenu = NULL;

	popupMenu.Attach( LocLoadMenu( MAKEINTRESOURCE(IDR_PROCESSES) ) );

	CPoint point;
	GetCursorPos( &point );

	switch ( GetInitCommand() )
	{
	case ID_EXTENSION_FINDUSEDMODULE:
	case ID_PROCESSES_MODULES:
		{
			BOOL bDependency = IsDependencyWalkerInstalled();
			pSubMenu = popupMenu.GetSubMenu(0);
			if( bDependency )
			{
				pSubMenu->EnableMenuItem( ID_MODULES_DEPENDENCY, MF_BYCOMMAND | MF_ENABLED );
			}
			else
			{
				pSubMenu->DeleteMenu( ID_MODULES_DEPENDENCY, MF_BYCOMMAND );
			}
		}
		break;

	case ID_PROCESSES_WINDOWS:
		pSubMenu = popupMenu.GetSubMenu(1);
		break;

	case ID_EXTENSION_FINDUSEDFILE:
		pSubMenu = popupMenu.GetSubMenu(2);
		break;

	case ID_PROCESSES_FILES:
		pSubMenu = popupMenu.GetSubMenu(2);
		break;

	case ID_EXTENSION_ALL_HANDLES:
	case ID_PROCESSES_HANDLES:
		{
			CString subItemText;
			pSubMenu = popupMenu.GetSubMenu(2);

			if ( m_SystemInfoList.GetSelectedSubItemText( COLUMN_HANDLE_TYPE, subItemText ) )
			{
				int nPos = -1;

				if ( subItemText == _T("Event") )
					nPos = 2;
				else if ( subItemText == _T("Mutant") )
					nPos = 3;
				else if ( subItemText == _T("WindowStation") )
					nPos = 4;
				else if ( subItemText == _T("Desktop") )
					nPos = 5;
				else if ( subItemText == _T("Process") )
					nPos = 6;
				else if ( subItemText == _T("Thread") )
					nPos = 7;

				if ( nPos != -1)
					pSubMenu->EnableMenuItem( nPos, MF_BYPOSITION | MF_ENABLED );
			}
		}
		break;

	case ID_PROCESSES_THREADS:
		pSubMenu = popupMenu.GetSubMenu(3);
		break;

	case ID_EXTENSION_SHOW_DRIVERS:
	case ID_PROCESSES_MEMORY:
		return;

	}

	if( pSubMenu != NULL )
	{
		pSubMenu->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, this, NULL );
	}
}

HANDLE CSystemInfoDlg::GetSelectedHandle()
{
   CString subItemText;
   HANDLE handle = 0;

   if ( !m_SystemInfoList.GetSelectedSubItemText( COLUMN_HANDLE, subItemText ) )
      return 0;

   _stscanf( (LPCTSTR)subItemText, _T("0x%X"), &handle );

   return handle;
}

DWORD CSystemInfoDlg::GetSelectedPID()
{
	DWORD processID = 0;

	if( IsInitMultiprocess() )
	{
		CString subItemText;
		if ( m_SystemInfoList.GetSelectedSubItemText( COLUMN_PID, subItemText ) )
		{
			processID = _ttoi( subItemText );
		}
	}
	else
	{
		if( GetInitCommand() == ID_PROCESSES_THREADS )
		{
			CString subItemText;
			if ( m_SystemInfoList.GetSelectedSubItemText( COLUMN_HANDLE_PID, subItemText ) )
			{
				processID = _ttoi( subItemText );
			}
		}
		else
		{
			processID = GetInitProcessId();
		}
	}
	return processID;
}

void CSystemInfoDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
//	Resize();
	CDialog::OnActivate(nState, pWndOther, bMinimized);
}

void CSystemInfoDlg::OnItemchangedListview(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	UINT uChangedState = pNMListView->uOldState ^ pNMListView->uNewState;
	if( ( uChangedState & LVIS_SELECTED ) != 0 )
	{
		RefreshStatus();
	}

	*pResult = 0;
}

void CSystemInfoDlg::OnFindText() 
{
	CFindTextDlg m_FindDlg;
	INT_PTR res = m_FindDlg.DoModal();
	if( res == IDOK )
	{
		m_FindOptions = m_FindDlg.m_options;
		OnFindNextText();
	}
}

void CSystemInfoDlg::OnFindNextText() 
{
	FindTextOnce( TRUE );
}

void CSystemInfoDlg::OnFindPrevText() 
{
	FindTextOnce( FALSE );
}

void CSystemInfoDlg::FindTextOnce( BOOL bLookForward )
{
	CString strFind = m_FindOptions.m_strFind;
	int iFirstItem = m_SystemInfoList.GetSelectedItem();
	int top = m_SystemInfoList.GetTopIndex();
	int page = m_SystemInfoList.GetCountPerPage();
	if( iFirstItem < top || iFirstItem >= top + page )
	{
		iFirstItem = top;
	}

	BOOL bForward = ( m_FindOptions.m_bForward && bLookForward ||
			!m_FindOptions.m_bForward && !bLookForward );

	int n = m_SystemInfoList.GetItemCount();
	int cols = m_SystemInfoList.GetHeaderCtrl()->GetItemCount();
	BOOL bFound = FALSE;

	for( int i=0; i<n; i++ )
	{
		if( strFind.IsEmpty() )
		{
			break;
		}

		int iEffective;
		if( bForward )
			iEffective = iFirstItem + 1 + i;
		else
			iEffective = iFirstItem - 1 - i;

		// if( iEffective >= n || iEffective < 0 ) ... loop over list

		int iItem = bForward ? iEffective % n : ( (iEffective + n) % n);

		for( int j=0; j<cols; j++ )
		{
			CString s = m_SystemInfoList.GetItemText( iItem, j );

			if( m_FindOptions.m_bMatchCase )
			{
				if( s.Find( strFind ) != -1 )
				{
					bFound = TRUE;
				}
			}
			else
			{
				if( FreeSearchInString( s, strFind ) )
				{
					bFound = TRUE;
				}
			}

			if( bFound )
			{
				m_SystemInfoList.SetSelectedItem( iItem );
				m_SystemInfoList.EnsureVisible( iItem, FALSE );

				i = n;
				break;
			}
		}
	}

	if( !bFound )
	{
		CString msg;
		msg.Format( LocLoadString(IDS_CANT_FIND_TEXT), strFind );
		AfxMessageBox( msg, MB_OK | MB_ICONWARNING );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///   Context menu handlers:     //////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSystemInfoDlg::OnHandlesRelease()
{
	HANDLE handle = (HANDLE)GetSelectedHandle();
	DWORD processID = GetSelectedPID();

	if ( processID == 0 )
		return;

	if ( handle != NULL )
	{
		DWORD dwFuncRetVal = FALSE;
		BOOL res = RemoteSimpleFunction( processID, (DWORD)handle, "CloseHandle", &dwFuncRetVal );
		BOOL bOK = res && dwFuncRetVal;
		if( bOK )
			MyWait( REASON_SIMPLE_FUNC );

		RefreshList();

		if( !bOK )
			AfxMessageBox( LocLoadString(IDS_CANT_RELEASE_HANDLE) );
	}
}

void CSystemInfoDlg::OnModulesLoad()
{
	CString  modulePath;
	DWORD processID = GetSelectedPID();

	if ( processID == 0 )
		return;

	if ( !m_SystemInfoList.GetSelectedSubItemText( COLUMN_MODULE_PATH, modulePath ) )
		return;

	using namespace RemoteExecute;
	DWORD dwRet = LoadDllForRemoteThread( processID,
		LOAD_LIBRARY, DONT_FREE_LIBRARY, NO_SPECIAL_MODE,
		(LPCTSTR)modulePath, NoFunction,
		NoFuncReturn, NoFuncReturnSpecial, NoLastError,
		DONT_RETURN_REMOTE_API_ERRORS, ZeroArguments, NoArguments );

	BOOL bOK = dwRet == 0;

	if( bOK )
		Sleep( REASON_REMOTE_THREAD );

	RefreshList();

	if( !bOK )
		AfxMessageBox( LocLoadString(IDS_CANT_LOAD_MODULE) );
}

void CSystemInfoDlg::OnModulesUnload()
{
	CString  modulePath;
	DWORD processID = GetSelectedPID();

	if ( processID == 0 )
		return;

	if ( !m_SystemInfoList.GetSelectedSubItemText( COLUMN_MODULE_PATH, modulePath ) )
		return;

	using namespace RemoteExecute;
	DWORD dwRet = LoadDllForRemoteThread( processID,
		DONT_LOAD_LIBRARY, FREE_LIBRARY, NO_SPECIAL_MODE,
		(LPCTSTR)modulePath, NoFunction,
		NoFuncReturn, NoFuncReturnSpecial, NoLastError,
		DONT_RETURN_REMOTE_API_ERRORS, ZeroArguments, NoArguments );

	BOOL bOK = dwRet == 0;

	if( bOK )
		Sleep( REASON_REMOTE_THREAD );

	RefreshList();

	if( !bOK )
		AfxMessageBox( LocLoadString(IDS_CANT_UNLOAD_MODULE) );
}

void CSystemInfoDlg::OnWindowsClose()
{
	HWND hWnd = (HWND)GetSelectedHandle();

	if ( hWnd )
	{
		::SendMessage( hWnd, WM_CLOSE, 0, 0 );
		Sleep( REASON_SEND_MESSAGE );
		RefreshList();
	}
}

void CSystemInfoDlg::OnHandlesEventSet()
{
	HANDLE handle = GetSelectedHandle();
	DWORD processID = GetSelectedPID();

	if ( handle != NULL )
	{
		DWORD dwFuncRetVal = FALSE;
		BOOL res = RemoteSimpleFunction( processID, (DWORD)handle, "SetEvent", &dwFuncRetVal );
		BOOL bOK = res && dwFuncRetVal;
		if( bOK )
			MyWait( REASON_SIMPLE_FUNC );

		RefreshList();

		if( !bOK )
			AfxMessageBox( LocLoadString(IDS_CANT_SET_EVENT) );
	}
}

void CSystemInfoDlg::OnHandlesEventReset()
{
	HANDLE handle = GetSelectedHandle();
	DWORD processID = GetSelectedPID();

	if ( handle != NULL )
	{
		DWORD dwFuncRetVal = FALSE;
		BOOL res = RemoteSimpleFunction( processID, (DWORD)handle, "ResetEvent", &dwFuncRetVal );
		BOOL bOK = res && dwFuncRetVal;

		if( bOK )
			MyWait( REASON_SIMPLE_FUNC );

		RefreshList();

		if( !bOK )
			AfxMessageBox( LocLoadString(IDS_CANT_RESET_EVENT) );
	}
}

void CSystemInfoDlg::OnHandlesEventPulse()
{
	HANDLE handle = GetSelectedHandle();
	DWORD processID = GetSelectedPID();

	if ( handle != NULL )
	{
		DWORD dwFuncRetVal = FALSE;
		BOOL res = RemoteSimpleFunction( processID, (DWORD)handle, "PulseEvent", &dwFuncRetVal );
		BOOL bOK = res && dwFuncRetVal;

		if( bOK )
			MyWait( REASON_SIMPLE_FUNC );

		RefreshList();

		if( !bOK )
			AfxMessageBox( LocLoadString(IDS_CANT_PULSE_EVENT) );
	}
}

void CSystemInfoDlg::OnHandlesMutantRelease()
{
	HANDLE handle = GetSelectedHandle();
	DWORD processID = GetSelectedPID();

	if ( handle != NULL )
	{
		DWORD dwFuncRetVal = FALSE;
		BOOL res = RemoteSimpleFunction( processID, (DWORD)handle, "ReleaseMutex", &dwFuncRetVal );
		BOOL bOK = res && dwFuncRetVal;

		if( bOK )
			MyWait( REASON_SIMPLE_FUNC );

		RefreshList();

		if( !bOK )
			AfxMessageBox( LocLoadString(IDS_CANT_RELEASE_MUTEX) );
	}
}

void CSystemInfoDlg::OnHandlesProcessExitprocess()
{
	HANDLE handle = (HANDLE)GetSelectedHandle();
	DWORD HandlePocessID = GetSelectedPID();
	DWORD processID = 0;
	SystemHandleInformation::GetProcessId( handle, processID, HandlePocessID );

	if ( processID == 0 )
		return;

	if ( handle != NULL )
	{
		DWORD dwFuncRetVal = FALSE;
		BOOL res = RemoteSimpleFunction( processID, 0, "ExitProcess", &dwFuncRetVal );
		BOOL bOK = res && dwFuncRetVal;

		if( bOK )
			MyWait( REASON_SIMPLE_FUNC );

		RefreshList();

		if( !bOK )
			AfxMessageBox( LocLoadString(IDS_CANT_EXIT_PROCESS) );
	}
}

void CSystemInfoDlg::OnHandlesProcessTerminate()
{
	HANDLE handle = (HANDLE)GetSelectedHandle();
	DWORD HandlePocessID = GetSelectedPID();
	DWORD processID = 0;
	SystemHandleInformation::GetProcessId( handle, processID, HandlePocessID );

	if ( processID == 0 )
		return;

	HANDLE hProcess = OpenProcess( PROCESS_TERMINATE, FALSE, processID );
	if( hProcess != NULL )
	{
		BOOL res = TerminateProcess( hProcess, 0 );
		BOOL bOK = res;
		CloseHandle( hProcess );

		if( bOK )
			MyWait( REASON_TERMINATE_PROCESS );

		if( !bOK )
			AfxMessageBox( LocLoadString(IDS_CANT_TERMINATE_PROCESS) );
	}
	else
	{
		AfxMessageBox( LocLoadString(IDS_CANT_OPEN_PROCESS) );
	}
	RefreshList();
}

void CSystemInfoDlg::OnHandlesThreadResume()
{
	HANDLE handle = (HANDLE)GetSelectedHandle();
	DWORD processID = GetSelectedPID();

	if ( processID == 0 )
		return;

	if ( handle != NULL )
	{
		DWORD dwFuncRetVal = FALSE;
		BOOL res = RemoteSimpleFunction( processID, (DWORD)handle, "ResumeThread", &dwFuncRetVal );
		BOOL bOK = res && dwFuncRetVal;

		if( bOK )
			MyWait( REASON_SIMPLE_FUNC );

		RefreshList();

		if( !bOK )
			AfxMessageBox( LocLoadString(IDS_CANT_RESUME_THREAD) );
	}
}

void CSystemInfoDlg::OnHandlesThreadSuspend()
{
	HANDLE handle = (HANDLE)GetSelectedHandle();
	DWORD processID = GetSelectedPID();

	if ( processID == 0 )
		return;

	if ( handle != NULL )
	{
		DWORD dwFuncRetVal = FALSE;
		BOOL res = RemoteSimpleFunction( processID, (DWORD)handle, "SuspendThread", &dwFuncRetVal );
		BOOL bOK = res && dwFuncRetVal;

		if( bOK )
			MyWait( REASON_SIMPLE_FUNC );

		RefreshList();

		if( !bOK )
			AfxMessageBox( LocLoadString(IDS_CANT_SUSPEND_THREAD) );
	}
}

void CSystemInfoDlg::OnHandlesThreadTerminate()
{
	HANDLE handle = (HANDLE)GetSelectedHandle();
	DWORD processID = GetSelectedPID();
	BOOL bOK = FALSE;

	if( GetInitCommand() == ID_PROCESSES_THREADS )
	{
		typedef HANDLE (WINAPI *POpenThread)(
				DWORD	dwDesiredAccess,
				BOOL	bInheritHandle,
				DWORD	dwThreadId
			);
		POpenThread OpenThread = NULL;
		HMODULE hKernel32 = GetModuleHandle( _T("kernel32.dll") );
		
		if( hKernel32 != NULL )
		{
			OpenThread = (POpenThread) GetProcAddress( hKernel32, "OpenThread" );
		}

//		if ( handle == NULL || processID == 0 )
		{
			CString subItemText;
			DWORD ThreadId = 0;

			if ( m_SystemInfoList.GetSelectedSubItemText( COLUMN_THREAD_TID, subItemText ) )
			{
				_stscanf( (LPCTSTR)subItemText, _T("0x%X"), &ThreadId );
			}

			if( OpenThread != 0 )
			{
				if( ThreadId != 0 )
				{
					HANDLE hThread = OpenThread( THREAD_TERMINATE, FALSE, ThreadId );
					if( hThread != NULL )
					{
						TRACE( _T("TerminateThread #1\n") );
						BOOL res = TerminateThread( hThread, 0 );
						TRACE( _T("TerminateThread #1... err = %d\n"), GetLastError() );
						CloseHandle( hThread );

						if( res )
						{
							bOK = TRUE;
						}
					}
					else
					{
						AfxMessageBox( LocLoadString(IDS_CANT_OPEN_THREAD) );
					}
				}
				else
				{
					AfxMessageBox( LocLoadString(IDS_INVALID_THREAD_ID) );
				}
			}
			else
			{
				AfxMessageBox( LocLoadString(IDS_CANT_FIND_OPENTHREAD_FUNC) );
			}
		}
	}

	if ( !bOK && handle != NULL && processID != 0 )
	{
		const DWORD dwArgumentCount = 2;
		DWORD dwArguments[dwArgumentCount] = { (DWORD)handle, 0 };

		TRACE( _T("TerminateThread #2\n") );
		DWORD dwFuncRetVal = FALSE;
		LONG LastError = 0;

		using namespace RemoteExecute;
		DWORD dwRet = LoadDllForRemoteThread( processID,
			LOAD_LIBRARY, FREE_LIBRARY, NO_SPECIAL_MODE,
			_T("kernel32.dll"), "TerminateThread",
			&dwFuncRetVal, NoFuncReturnSpecial, &LastError,
			DONT_RETURN_REMOTE_API_ERRORS,
			dwArgumentCount, dwArguments );
		bOK = dwRet == 0 && dwFuncRetVal;

		if( dwRet == 0x100 ) // Hardcoded constant from LoadDll.cpp
		{
			// We terminated last thread of the process
			bOK = TRUE;
		}

		if( !bOK )
		{
			HANDLE hLocalHandle = SystemHandleInformation::DuplicateHandle( processID, handle );
			if( hLocalHandle != NULL )
			{
				TRACE( _T("TerminateThread #3 (0x%X)\n"), hLocalHandle );
				BOOL res = TerminateThread( hLocalHandle, 0 );
				TRACE( _T("TerminateThread #3... err = %d\n"), GetLastError() );
				CloseHandle( hLocalHandle );

				if( res )
				{
					bOK = TRUE;
				}
			}
			else
			{
				AfxMessageBox( LocLoadString(IDS_CANT_OPEN_PROCESS) );
			}
		}
		//TRACE( _T("LoadDllForRemoteThread> TerminateThread: PID = %d, dwRet = 0x%X, dwFuncRetVal = %d, LastError = %d\n"),
		//	processID, dwRet, dwFuncRetVal, LastError );
	}

	if( bOK )
		MyWait( REASON_REMOTE_THREAD );

	RefreshList();

	if( !bOK )
		AfxMessageBox( LocLoadString(IDS_CANT_TERMINATE_THREAD) );
}

void CSystemInfoDlg::OnHandlesWindowstationAssigntoprocess()
{
	HANDLE handle = (HANDLE)GetSelectedHandle();
	DWORD processID = GetSelectedPID();

	if ( processID == 0 )
		return;

	if ( handle != NULL )
	{
		const DWORD dwArgumentCount = 1;
		DWORD dwArguments[dwArgumentCount] = { (DWORD)handle };

		DWORD dwFuncRetVal = FALSE;
		LONG LastError = 0;

		using namespace RemoteExecute;
		DWORD dwRet = LoadDllForRemoteThread( processID,
			LOAD_LIBRARY, FREE_LIBRARY, NO_SPECIAL_MODE,
			_T("user32.dll"), "SetProcessWindowStation",
			&dwFuncRetVal, NoFuncReturnSpecial, &LastError,
			DONT_RETURN_REMOTE_API_ERRORS,
			dwArgumentCount, dwArguments );
		BOOL bOK = dwRet == 0 && dwFuncRetVal;

		if( bOK )
			MyWait( REASON_REMOTE_THREAD );

		RefreshList();

		if( !bOK )
			AfxMessageBox( LocLoadString(IDS_CANT_ASSIGN_WINSTATION) );
	}
}

void CSystemInfoDlg::OnHandlesDesktopSwithto()
{
	HANDLE handle = (HANDLE)GetSelectedHandle();
	DWORD processID = GetSelectedPID();

	if ( processID == 0 )
		return;

	if ( handle != NULL )
	{
		const DWORD dwArgumentCount = 1;
		DWORD dwArguments[dwArgumentCount] = { (DWORD)handle };

		DWORD dwFuncRetVal = FALSE;
		LONG LastError = 0;

		using namespace RemoteExecute;
		DWORD dwRet = LoadDllForRemoteThread( processID,
			LOAD_LIBRARY, FREE_LIBRARY, NO_SPECIAL_MODE,
			_T("user32.dll"), "SwitchDesktop",
			&dwFuncRetVal, NoFuncReturnSpecial, &LastError,
			DONT_RETURN_REMOTE_API_ERRORS,
			dwArgumentCount, dwArguments );
		BOOL bOK = dwRet == 0 && dwFuncRetVal;

		if( bOK )
			MyWait( REASON_REMOTE_THREAD );

		RefreshList();

		if( !bOK )
			AfxMessageBox( LocLoadString(IDS_CANT_SWITCH_DESKTOP) );
	}
}

void CSystemInfoDlg::OnModulesFileproperties()
{
	CString  modulePath;
	DWORD processID = GetSelectedPID();

	if ( processID == 0 )
		return;

	if ( !m_SystemInfoList.GetSelectedSubItemText( COLUMN_MODULE_PATH, modulePath ) )
		return;

	MyShellExecute( this->GetSafeHwnd(), SHELL_OPERATION_FILE_PROPERTIES, modulePath );
}

void CSystemInfoDlg::OnModulesDependency()
{
	CString  modulePath;
	DWORD processID = GetSelectedPID();

	if ( processID == 0 )
		return;

	if ( !m_SystemInfoList.GetSelectedSubItemText( COLUMN_MODULE_PATH, modulePath ) )
		return;

	MyShellExecute( this->GetSafeHwnd(), SHELL_OPERATION_VIEW_DEPENDENCY, modulePath );
}

void CSystemInfoDlg::OnMemoryExpand() 
{
	m_bExpandRegions = TRUE;
	RefreshList();
}

void CSystemInfoDlg::OnMemoryCompact() 
{
	m_bExpandRegions = FALSE;
	RefreshList();
}

void CSystemInfoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		if( cxIcon == 16 && cyIcon == 16 )
		{
			dc.DrawIcon( x, y, theApp.hMainIconSmall );
		}
		else
		{
			dc.DrawIcon( x, y, theApp.hMainIconBig );
		}
	}
	else
	{
		CDialog::OnPaint();
	}
}

//////////////////////////////////////////////////////////////////////////

void CSystemInfoDlg::OnHandlesSecurity() 
{
	HANDLE handle = (HANDLE)GetSelectedHandle();
	DWORD processID = GetSelectedPID();

	if ( processID == 0 )
		return;

	HANDLE hLocal = SystemHandleInformation::DuplicateHandle( processID, handle );
	if( hLocal == NULL )
		return;

	CString strName;
	if( GetInitCommand() == ID_PROCESSES_FILES )
		m_SystemInfoList.GetSelectedSubItemText( COLUMN_FILE_DEVICE, strName );
	else
		m_SystemInfoList.GetSelectedSubItemText( COLUMN_HANDLE_NAME, strName );

	OB_TYPE_ENUM type = OB_TYPE_UNKNOWN;

	if( GetInitCommand() == ID_PROCESSES_FILES )
	{
		type = OB_TYPE_FILE;
	}
	else
	{
		CString strType;
		m_SystemInfoList.GetSelectedSubItemText( COLUMN_HANDLE_TYPE, strType );
		DecodeWinObjectType( strType, type );

	}

	if( type == OB_TYPE_FILE )
	{
		if( beginsi( strName, _T("\\Device\\NamepPipe\\Win32Pipes.") ) )
		{
			type = OB_TYPE_ANONPIPE;
		}
		else if( beginsi( strName, _T("\\Device\\NamepPipe\\") ) )
		{
			type = OB_TYPE_NAMEDPIPE;
		}
	}

	ObjectInformation info;
	info.m_hHandle = hLocal;
	info.m_szName[0] = _T('\0');
	_tcsncpy( info.m_szObjectName, strName, SIZEOF_ARRAY(info.m_szObjectName) );
	info.m_objInternalType = type;

	CSecurityInformation* pSec = CSecurityInformation::CreateInstance( info, FALSE );
	if( pSec != NULL )
	{
		pSec->EditSecurity( this->GetSafeHwnd() );
		pSec->Release();
	}

	CloseHandle( hLocal );
}

void CSystemInfoDlg::OnModulesSecurity() 
{
	CString strName;
	m_SystemInfoList.GetSelectedSubItemText( COLUMN_MODULE_PATH, strName );

//	HANDLE hLocal = CreateFile( strName, GENERIC_READ, FILE_SHARE_READ, NULL,
//		OPEN_ALWAYS, 0, NULL );
//	if( hLocal == INVALID_HANDLE_VALUE )
//		return;

	ObjectInformation info;
	info.m_hHandle = NULL;
	//info.m_szName[0] = _T('\0');
	_tcsncpy( info.m_szName, strName, SIZEOF_ARRAY(info.m_szName) );
	_tcsncpy( info.m_szObjectName, strName, SIZEOF_ARRAY(info.m_szObjectName) );
	info.m_objInternalType = OB_TYPE_FILE;

	CSecurityInformation* pSec = CSecurityInformation::CreateInstance( info, FALSE );
	if( pSec != NULL )
	{
		pSec->EditSecurity( this->GetSafeHwnd() );
		pSec->Release();
	}

//	CloseHandle( hLocal );
}

void CSystemInfoDlg::OnHandlesThreadSecurity() 
{
	HANDLE handle = (HANDLE)GetSelectedHandle();
	DWORD processID = GetSelectedPID();

	if ( processID == 0 )
		return;

	HANDLE hLocal = SystemHandleInformation::DuplicateHandle( processID, handle );
	if( hLocal == NULL )
		return;

	CString strName;
	m_SystemInfoList.GetSelectedSubItemText( COLUMN_THREAD_TID, strName );

	ObjectInformation info;
	info.m_hHandle = hLocal;
	info.m_szName[0] = _T('\0');
	_tcsncpy( info.m_szObjectName, strName, SIZEOF_ARRAY(info.m_szObjectName) );
	info.m_objInternalType = OB_TYPE_THREAD;

	CSecurityInformation* pSec = CSecurityInformation::CreateInstance( info, FALSE );
	if( pSec != NULL )
	{
		pSec->EditSecurity( this->GetSafeHwnd() );
		pSec->Release();
	}

	CloseHandle( hLocal );
}

//////////////////////////////////////////////////////////////////////////

