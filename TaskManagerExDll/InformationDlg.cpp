// ProcessInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "InformationDlg.h"
#include "TaskManagerExDll.h"
#include "Localization.h"
#include "AboutExtension.h"

#include "LoadDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE( CInformationDlgThread, CWinThread )

BOOL CInformationDlgThread::InitInstance()
{
	SetThreadNativeLanguage();
	CInformationDlg dlg;
	m_pMainWnd = &dlg;

	dlg.DoModal();

	PostQuitMessage( 0 ); // quit thread gracefully
	return TRUE;
//	return FALSE; // return error to quit thread
}

BOOL CInformationDlgThread::Initialize( DWORD pID, WPARAM command )
{
	m_processID = pID;
	m_command = command;

	return TRUE;
}

CInformationDlgThread* CInformationDlgThread::Start( DWORD pID, WPARAM command )
{
   CInformationDlgThread* _this = (CInformationDlgThread*)::AfxBeginThread(
                  RUNTIME_CLASS(CInformationDlgThread),
                  THREAD_PRIORITY_NORMAL,
                  0,
                  CREATE_SUSPENDED );

   _this->Initialize( pID, command );

   _this->ResumeThread();

   return _this;
}

/////////////////////////////////////////////////////////////////////////////
// CInformationDlg dialog


CInformationDlg::CInformationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInformationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInformationDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_OldClientSize = CSize(0,0);
}


void CInformationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInformationDlg)
	DDX_Control(pDX, IDC_EXIT, m_wndExit);
	DDX_Control(pDX, IDC_INFO, m_wndInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInformationDlg, CDialog)
	//{{AFX_MSG_MAP(CInformationDlg)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_WM_HELPINFO()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInformationDlg message handlers

void CInformationDlg::OnClose()
{
	CDialog::OnClose();
}

void CInformationDlg::OnDestroy()
{
	CDialog::OnDestroy();

	m_Font.DeleteObject();
}

void CInformationDlg::OnOK()
{
//	CDialog::OnOK();
//	OnRefresh();
}

void CInformationDlg::OnCancel()
{
	CDialog::OnCancel();
}

void CInformationDlg::OnExit()
{
	EndDialog( IDOK );
}

/////////////////////////////////////////////////////////////////////////////

BOOL CInformationDlg::PreTranslateMessage(MSG* pMsg) 
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}

	return CDialog::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////

BOOL CInformationDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	pHelpInfo = pHelpInfo;
	CAboutExtensionThread::Start( this );
	return TRUE;
	//return CDialog::OnHelpInfo(pHelpInfo);
}

/////////////////////////////////////////////////////////////////////////////

int CInformationDlg::DoModal() 
{
	// load resource as necessary
	if( m_lpszTemplateName != NULL && m_lpDialogTemplate == NULL )
	{
		m_lpDialogTemplate = LocLoadDialog( m_lpszTemplateName );
		m_lpszTemplateName = NULL;
	}

	return CDialog::DoModal();
}

/////////////////////////////////////////////////////////////////////////////

void CInformationDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if( !IsIconic() )
	{
		if( ::IsWindow( m_wndInfo.m_hWnd ) )
		{
			CRect r;
			m_wndInfo.GetWindowRect( &r );
			ScreenToClient( &r );
			r.right += cx - m_OldClientSize.cx;
			r.bottom += cy - m_OldClientSize.cy;
			m_wndInfo.MoveWindow( &r );
		}

		if( ::IsWindow( m_wndExit.m_hWnd ) )
		{
			CRect r;
			m_wndExit.GetWindowRect( &r );
			ScreenToClient( &r );
			r += CPoint( cx - m_OldClientSize.cx, 0 );
			m_wndExit.MoveWindow( &r );
			m_wndExit.RedrawWindow();
		}

		m_OldClientSize.cx = cx;
		m_OldClientSize.cy = cy;
	}
}

/////////////////////////////////////////////////////////////////////////////

void CInformationDlg::OnGetMinMaxInfo( MINMAXINFO* pMinMaxInfo )
{
	CDialog::OnGetMinMaxInfo( pMinMaxInfo );
	pMinMaxInfo->ptMinTrackSize = CPoint( 300, 200 );
}

/////////////////////////////////////////////////////////////////////////////

BOOL CInformationDlg::OnInitDialog() 
{
	CInformationDlgThread* pThread = (CInformationDlgThread*)AfxGetThread();

	CDialog::OnInitDialog();

	SetIcon( theApp.hMainIconBig, TRUE );
	SetIcon( theApp.hMainIconSmall, FALSE );
//	SetClassLong( this->GetSafeHwnd(), GCL_HICON, (long)theApp.hMainIconBig );		// this changes icon for any dialog in the process!
//	SetClassLong( this->GetSafeHwnd(), GCL_HICONSM, (long)theApp.hMainIconSmall );	// this changes icon for any dialog in the process!

	bool bAlwaysOnTop = theApp.IsTaskManagerWindowTopMost();
	if( bAlwaysOnTop )
	{
		::SetWindowPos( *this, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
	}

	m_Font.CreateFont(16, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0,	0,
		_T("Courier New") );

	const int iCharTabStop = 4;
	const int iDlgTabStop = (iCharTabStop*32+3)/6; // Only for 16x "Courier New"!

	m_wndInfo.SetTabStops( iDlgTabStop );
	m_wndInfo.SetFont( &m_Font );


	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.Activate(TRUE);

		m_tooltip.AddTool( GetDlgItem(IDC_EXIT), LocLoadString(IDC_INFO_EXIT) );
		m_tooltip.AddTool( GetDlgItem(IDC_REFRESH ), LocLoadString(IDC_INFO_REFRESH) );

		// TODO: Use one of the following forms to add controls:
		// m_tooltip.AddTool(GetDlgItem(IDC_<name>), <string-table-id>);
		// m_tooltip.AddTool(GetDlgItem(IDC_<name>), "<text>");
	}

	CString strCaption = _T("Unknown");

	switch( pThread->m_command )
	{
	case ID_PROCESSES_INFO:
		{
			TCHAR szFileName[MAX_PATH] = _T("");
			GetProcessExecutableName( pThread->m_processID, szFileName, SIZEOF_ARRAY(szFileName) );
			CString s = szFileName;
			int pos = s.ReverseFind( _T('\\') );

			strCaption.Format( _T("PID = %d(0x%X) - %s - "),
				pThread->m_processID, pThread->m_processID, s.Mid(pos+1) );
		}
		break;
	}

	switch( pThread->m_command )
	{
	case ID_PROCESSES_INFO:

		strCaption += LocLoadString(IDS_INFORMATION_RPOCESS);
		break;
	}

	SetWindowText( strCaption );
	SetWindowPos( NULL, 0, 0, 700, 600, SWP_NOMOVE | SWP_NOZORDER );

	OnRefresh();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CInformationDlg::OnRefresh()
{
	CInformationDlgThread* pThread = (CInformationDlgThread*)AfxGetThread();

	int x = m_wndInfo.GetScrollPos( SB_HORZ );
	int y = m_wndInfo.GetScrollPos( SB_VERT );

	int iCharWidth = 1;
	{
		CDC dc;
		dc.CreateCompatibleDC( GetDC() );
		CFont* pOldFont = dc.SelectObject( &m_Font );
		iCharWidth = dc.GetTextExtent( _T(" ") ).cx;
		dc.SelectObject( pOldFont );
		dc.DeleteDC();
	}

	CString strInformation;
	BOOL res = FALSE;

	switch( pThread->m_command )
	{
	case ID_PROCESSES_INFO:
		{
			res = GetProcessInformation( strInformation );
		}
		break;
	}

	strInformation.Replace( _T("\n"), _T("\r\n") );
	m_wndInfo.SetWindowText( strInformation );
	m_wndInfo.LineScroll( y, x/iCharWidth );
}

/////////////////////////////////////////////////////////////////////////////

struct FTIME
{
	FTIME()
	{
		li.QuadPart = 0;
	}

	FTIME( const FILETIME& ft )
	{
		li.LowPart = ft.dwLowDateTime;
		li.HighPart = ft.dwHighDateTime;
	}

	FTIME( const LONGLONG& l64 )
	{
		li.QuadPart = l64;
	}

	FTIME( const LARGE_INTEGER& li2 )
	{
		li.QuadPart = li2.QuadPart;
	}

	operator FILETIME()
	{
		FILETIME ft;
		ft.dwLowDateTime = li.LowPart;
		ft.dwHighDateTime = li.HighPart;
		return ft;
	}

	operator LONGLONG()
	{
		return li.QuadPart;
	}

	operator LARGE_INTEGER()
	{
		return li;
	}

	LARGE_INTEGER li;
};

CString GetPeriod( LONGLONG l64 ) // IN: FILETIME
{
	CString s;
	LONGLONG milliseconds = l64/10000;
	DWORD hours = (DWORD) milliseconds / (3600 * 1000);
	DWORD min = (DWORD) (milliseconds / (60 * 1000)) % 60;
	DWORD sec = (DWORD) (milliseconds / (1000)) % 60;
	DWORD ms = (DWORD) milliseconds % 1000;
	s.Format( _T("%d:%02d:%02d.%03d"), hours, min, sec, ms );
	return s;
}

struct FLAG_INFO
{
	int iFlag;
	LPCTSTR szFlagName;
	LPCTSTR szDescription;
};

#define FLAG(x, d)	{ x, _T(#x), d },
#define NULL_FLAG	{ 0, NULL, NULL }

FLAG_INFO flagsServiceTypeFlags[] =
{
	FLAG( SERVICE_FILE_SYSTEM_DRIVER, _T("The service is a file system driver.") )
	FLAG( SERVICE_KERNEL_DRIVER, _T("The service is a device driver.") )
	FLAG( SERVICE_WIN32_OWN_PROCESS, _T("The service runs in its own process.") )
	FLAG( SERVICE_WIN32_SHARE_PROCESS, _T("The service shares a process with other services.") )
	FLAG( SERVICE_INTERACTIVE_PROCESS, _T("The service can interact with the desktop.") )
	NULL_FLAG
};

FLAG_INFO flagsServiceStatusValues[] =
{
	FLAG( SERVICE_STOPPED, _T("The service is not running.") )
	FLAG( SERVICE_START_PENDING, _T("The service is starting.") )
	FLAG( SERVICE_STOP_PENDING, _T("The service is stopping.") )
	FLAG( SERVICE_RUNNING, _T("The service is running.") )
	FLAG( SERVICE_CONTINUE_PENDING, _T("The service continue is pending.") )
	FLAG( SERVICE_PAUSE_PENDING, _T("The service pause is pending.") )
	FLAG( SERVICE_PAUSED, _T("The service is paused.") )
	NULL_FLAG
};

FLAG_INFO flagsServiceAcceptFlags[] =
{
	FLAG( SERVICE_ACCEPT_NETBINDCHANGE, _T("The service is a network component that can accept changes in its binding without being stopped and restarted.") )
	FLAG( SERVICE_ACCEPT_PARAMCHANGE, _T("The service can reread its startup parameters without being stopped and restarted.") )
	FLAG( SERVICE_ACCEPT_PAUSE_CONTINUE, _T("The service can be paused and continued.") )
	FLAG( SERVICE_ACCEPT_SHUTDOWN, _T("The service is notified when system shutdown occurs.") )
	FLAG( SERVICE_ACCEPT_STOP, _T("The service can be stopped.") )
	FLAG( SERVICE_ACCEPT_HARDWAREPROFILECHANGE, _T("The service is notified when the computer's hardware profile has changed.") )
	FLAG( SERVICE_ACCEPT_POWEREVENT, _T("The service is notified when the computer's power status has changed.") )
	FLAG( SERVICE_ACCEPT_SESSIONCHANGE, _T("The service is notified when the computer's session status has changed.") )
	NULL_FLAG
};


FLAG_INFO flagsServiceFlags[] =
{
	FLAG( 0, _T("The service is running in a process that is not a system process, or it is not running.") )
	FLAG( SERVICE_RUNS_IN_SYSTEM_PROCESS, _T("The service runs in a system process that must always be running.") )
	NULL_FLAG
};

tstring DecodeFlags( DWORD x, FLAG_INFO* pInfo, bool bMultipleFlags, bool bAdvanced, LPCTSTR szPrefix )
{
	DWORD Value = x;
	std::vector<FLAG_INFO*> arrFlags;
	for( FLAG_INFO* p = pInfo; p->szFlagName != NULL; p++ )
	{
		DWORD flag = p->iFlag;
		if( bMultipleFlags )
		{
			if( flag == 0 )
			{
				if( x == 0 )
				{
					arrFlags.push_back( p );
				}
			}
			else if( 0 != (flag & Value) )
			{
				Value &= ~flag;
				arrFlags.push_back( p );
			}
		}
		else
		{
			if( flag == Value )
			{
				Value = 0;
				arrFlags.push_back( p );
			}
		}
	}

	tstring sUnknown;
	tstring sUnknownName;
	if( Value != 0 )
	{
		TCHAR buf[100] = _T("");
		_stprintf( buf, _T("0x%08X"), Value );
		sUnknown = buf;
		sUnknownName = _T("Unknown_") + sUnknown;
	}
	FLAG_INFO fUnknown = { 0, sUnknown.c_str(), sUnknownName.c_str() };
	if( Value != 0 )
	{
		arrFlags.push_back( &fUnknown );
	}

	FLAG_INFO fEmpty = { 0, _T("<empty>"), _T("<empty value>")  };
	if( arrFlags.size() == 0 && x==0 )
	{
		arrFlags.push_back( &fEmpty );
	}

	tstring s;
	for( size_t i=0; i<arrFlags.size(); i++ )
	{
		FLAG_INFO& info = *arrFlags[i];

		if( bAdvanced )
		{
			TCHAR buf[100] = _T("");
			_stprintf( buf, _T("%-25s"), info.szFlagName );

			s += szPrefix;
			s += buf;
			s += _T(" - ");
			s += info.szDescription;
			s += _T("\n");
		}
		else
		{
			if( s.length() > 0 )
			{
				s += _T(" | ");
			}
			s += info.szFlagName;
		}
	}

	return s;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CInformationDlg::GetProcessInformation( CString& info )
{
	CInformationDlgThread* pThread = (CInformationDlgThread*)AfxGetThread();
	info.Empty();

	// Get teh process list
	SystemProcessInformation spi( pThread->m_processID, TRUE, TRUE );

	DWORD pID;
	SystemProcessInformation::PROCESS_INFO pi;
	ZeroMemory( &pi, sizeof(pi) );

	// Iterating through the processes and get the module list
	for ( POSITION pos = spi.m_ProcessInfos.GetStartPosition(); pos != NULL; )
	{
		spi.m_ProcessInfos.GetNextAssoc( pos, pID, pi );
		break; // I hope there is only one process with PID == m_processId 8-)
	}

	CString d;
	TCHAR szBuf[MAX_PATH] = _T("");
	CTime t;

	SYSTEMTIME st;
	GetLocalTime( &st );

	FILETIME ft;
	SystemTimeToFileTime( &st, &ft );

	FILETIME ft2;
	GetSystemTimeAsFileTime( &ft2 );

	FTIME	time1 = ft;
	FTIME	time2 = ft2;

	FTIME	ftCreation;
	FTIME	ftKernel;
	FTIME	ftUser;

	tstring sCommandLine;

	{
		DWORD dwFuncRetVal = FALSE;
		LONG LastError = 0;

		DWORD dwSpecial = 0;

		using namespace RemoteExecute;
		DWORD dwRet = LoadDllForRemoteThread( pi.processId,
			LOAD_LIBRARY, FREE_LIBRARY, GET_COMMAND_LINE,
			_T("kernel32.dll"), "GetCommandLine" FUNC_SUFFIX,
			&dwFuncRetVal, &dwSpecial, &LastError,
			DONT_RETURN_REMOTE_API_ERRORS,
			ZeroArguments, NoArguments );

		BOOL bOK = dwRet == 0;

		if( bOK && dwFuncRetVal != 0 )
		{
			TCHAR* pCmdLine = (TCHAR*) dwFuncRetVal;
			sCommandLine = pCmdLine;
			FreeSpecialBuffer( dwFuncRetVal );
		}
	}

//#pragma  warning (push, 3)
//#pragma  warning (disable: 4786)
	std::vector<tstring> arrEnvironment;
//#pragma  warning (pop)
	{
		DWORD dwFuncRetVal = FALSE;
		LONG LastError = 0;

		DWORD dwSpecial = 0;

		using namespace RemoteExecute;
		DWORD dwRet = LoadDllForRemoteThread( pi.processId,
			LOAD_LIBRARY, FREE_LIBRARY, GET_ENVIRONMENT,
			_T("kernel32.dll"), "GetEnvironmentStrings" FUNC_SUFFIX_ALT,
			&dwFuncRetVal, &dwSpecial, &LastError,
			DONT_RETURN_REMOTE_API_ERRORS,
			ZeroArguments, NoArguments );

		BOOL bOK = dwRet == 0;

		if( bOK && dwFuncRetVal != 0 )
		{
			TCHAR* pEnv = (TCHAR*) dwFuncRetVal;

			while( *pEnv )
			{
				tstring s = pEnv;
				arrEnvironment.push_back( s );
				pEnv += s.length() + 1;
			}

			FreeSpecialBuffer( dwFuncRetVal );
		}
	}

	std::vector<MY_SERVICE_INFO> arrServices;
	
	DWORD dwSCManagerProcessId = IS_PID_SYSTEM( pi.processId ) ? 0 : pi.processId;

	// Enumerate all Windows Services running in the current process:
	if( IAdvapi32::EnumServicesStatusEx != NULL )
	{
		SC_HANDLE hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE );
		if( hSCManager != NULL )
		{
			const DWORD dwServiceType = SERVICE_WIN32 | SERVICE_DRIVER;
			const DWORD dwServiceState = SERVICE_ACTIVE; // SERVICE_STATE_ALL;
			DWORD dwBytesNeeded = 0;
			DWORD dwServiceReturned = 0;
			DWORD dwResumeHandle = 0;
			BYTE buf[64*1024];
			const DWORD dwSize = sizeof(buf);
			IAdvapi32::LPENUM_SERVICE_STATUS_PROCESS pEnum = (IAdvapi32::LPENUM_SERVICE_STATUS_PROCESS) &buf;
			BOOL res;
			LONG err;
			DWORD i;

			while(TRUE)
			{
				res = IAdvapi32::EnumServicesStatusEx( hSCManager,
					IAdvapi32::SC_ENUM_PROCESS_INFO,
					dwServiceType, dwServiceState, buf, dwSize,
					&dwBytesNeeded, &dwServiceReturned, &dwResumeHandle, NULL );
				err = GetLastError();

				if( res || ( !res && err == ERROR_MORE_DATA ) )
				{
					for( i=0; i<dwServiceReturned; i++ )
					{
						IAdvapi32::ENUM_SERVICE_STATUS_PROCESS& info = pEnum[i];
						if( info.ServiceStatusProcess.dwProcessId == dwSCManagerProcessId )
						{
							MY_SERVICE_INFO info2;
							info2.sName = info.lpServiceName;
							info2.sDisplay = info.lpDisplayName;
							info2.ServiceStatusProcess = info.ServiceStatusProcess;
							arrServices.push_back( info2 );
						}
					}
				}

				if( !res && err == ERROR_MORE_DATA )
				{
					continue;
				}

				break;
			}

			CloseServiceHandle( hSCManager );
			hSCManager = NULL;
		}
		else
		{
			TRACE( _T("TaskManagerEx: Can't open service manager! (err - %d)\n"), GetLastError() );
		}
	}
	else
	{
		TRACE( _T("TaskManagerEx: IAdvapi32::EnumServicesStatusEx is not ready!\n") );
	}

//#pragma message (WARNING "Lots of hardcoded strings!")

	d.Format( _T("Process \"%ls\" (PID = %d) information:\n"), pi.spi.ProcessName.Buffer, pi.processId );
	info += d;
	d.Format( _T("=================================================\n\n") );
	info += d;

	d.Format( _T("Exe image name:\t\t\t\"%s\"\n"), pi.szExe );
	info += d;

	SystemMemoryMapInformation::FileFromAddress( pThread->m_processID, NULL, szBuf, SIZEOF_ARRAY(szBuf) );
	if( 0 != _tcsicmp( szBuf, pi.szExe ) )
	{
		d.Format( _T("Exe file (ORIGINAL!!!):\t\"%s\"\n"), szBuf );
		info += d;
	}

	if( sCommandLine.length() > 0 )
	{
		d.Format( _T("Command line:\t\t\t%s\n"), sCommandLine.c_str() );
		info += d;
	}

	GetProcessExecutableName( pi.spi.InheritedFromProcessId, szBuf, SIZEOF_ARRAY(szBuf) );
	d.Format( _T("Parent process:\t\t\tPID = %d, \"%s\"\n"), pi.spi.InheritedFromProcessId, szBuf );
	info += d;

	d.Format( _T("Base priority:\t\t\t%d; priority boost: %s\n"), pi.spi.BasePriority,
		(pi.bDisablePriorityBoost ? _T("Disabled") : _T("Enabled") ) );
	info += d;

	d.Format( _T("CPU affinity mask:\t\t0x%X; system CPU affinity mask: 0x%X\n"),
		pi.dwProcessAffinity, pi.dwSystemAffinity );
	info += d;

	d.Format( _T("Image version:\t\t\t%d.%d\n"),
		HIWORD(pi.dwVersion), LOWORD(pi.dwVersion) );
	info += d;

	d.Format( _T("Working set:\t\t\t%d, min = %d, max = %d\n"),
		pi.pmc.WorkingSetSize, pi.minWorkSet, pi.maxWorkSet );
	info += d;

//////////////////////////////////////////////////////////////////////////
/*
	ftCreation = pi.spi.CreateTime;
	ftKernel = pi.spi.KernelTime.QuadPart * 10;
	ftUser = pi.spi.UserTime.QuadPart * 10;

	if( ftCreation != 0 )
	{
		t = (FILETIME)ftCreation;
		d.Format( _T("Process create time:\t%s\n"), t.Format( _T("%#c") ) );
		info += d;

		d.Format( _T("Process age:\t\t\t%s\n"), GetPeriod(time2 - ftCreation) );
		info += d;
	}

	d.Format( _T("Process kernel time:\t%s\n"), GetPeriod(ftKernel) );
	info += d;

	d.Format( _T("Process user time:\t\t%s\n"), GetPeriod(ftUser) );
	info += d;

	d.Format( _T("Process kernel + user time:\t%s\n"), GetPeriod(ftKernel + ftUser) );
	info += d;
//*/
//////////////////////////////////////////////////////////////////////////

	ftCreation = pi.ftCreation;
	ftKernel = pi.ftKernel;
	ftKernel = ftKernel * 10;
	ftUser = pi.ftUser;
	ftUser = ftUser * 10;

	if( ftCreation != 0 )
	{
		t = (FILETIME)ftCreation;
		d.Format( _T("Process create time:\t%s\n"), t.Format( _T("%#c") ) );
		info += d;

		d.Format( _T("Process age:\t\t\t%s\n"), GetPeriod(time2 - ftCreation) );
		info += d;
	}

	d.Format( _T("Process kernel time:\t%s\n"), GetPeriod(ftKernel) );
	info += d;

	d.Format( _T("Process user time:\t\t%s\n"), GetPeriod(ftUser) );
	info += d;

	d.Format( _T("Process kernel + user time:\t%s\n"), GetPeriod(ftKernel + ftUser) );
	info += d;

//////////////////////////////////////////////////////////////////////////

	d.Format( _T("==================================\n") );
	info += d;

	if( arrServices.size() > 0 )
	{
		d.Format( _T("Services:\n") );
		info += d;

		for( size_t i=0; i<arrServices.size(); i++ )
		{
			MY_SERVICE_INFO& info2 = arrServices[i];
			IAdvapi32::SERVICE_STATUS_PROCESS& ssp = info2.ServiceStatusProcess;
			TCHAR szBuf[200] = _T("");

			_sntprintf( szBuf, SIZEOF_ARRAY(szBuf), _T("%s (%s):"),
				info2.sName.c_str(), info2.sDisplay.c_str() );

			d.Format( _T("\t\t%-30s type = %s; state = %s; accepted = %s; flags = %s;\n"),
				szBuf,
				DecodeFlags( ssp.dwServiceType, flagsServiceTypeFlags, true, false, NULL ).c_str(),
				DecodeFlags( ssp.dwCurrentState, flagsServiceStatusValues, true, false, NULL ).c_str(),
				DecodeFlags( ssp.dwControlsAccepted, flagsServiceAcceptFlags, true, false, NULL ).c_str(),
				DecodeFlags( ssp.dwServiceFlags, flagsServiceFlags, true, false, NULL ).c_str()
				);

			info += d;
		}
	}
	else
	{
		d.Format( _T("Services:\t\t\t\tThere are no services in this process!\n") );
		info += d;
	}

	if( arrServices.size() > 0 )
	{
		d.Format( _T("==================================\n") );
		info += d;
		d.Format( _T("Services (full information):\n") );
		info += d;

		for( size_t i=0; i<arrServices.size(); i++ )
		{
			MY_SERVICE_INFO& info2 = arrServices[i];
			IAdvapi32::SERVICE_STATUS_PROCESS& ssp = info2.ServiceStatusProcess;
			TCHAR szBuf[200] = _T("");

			_sntprintf( szBuf, SIZEOF_ARRAY(szBuf), _T("%s (%s):"),
				info2.sName.c_str(), info2.sDisplay.c_str() );

			//LPCTSTR szPref = _T("\t");

			d.Format( _T("Service: \t%-30s\n%s%s%s%s\n"),
				szBuf,
				DecodeFlags( ssp.dwServiceType, flagsServiceTypeFlags, true, true, _T("type:\t") ).c_str(),
				DecodeFlags( ssp.dwCurrentState, flagsServiceStatusValues, true, true, _T("status:\t") ).c_str(),
				DecodeFlags( ssp.dwControlsAccepted, flagsServiceAcceptFlags, true, true, _T("accept:\t") ).c_str(),
				DecodeFlags( ssp.dwServiceFlags, flagsServiceFlags, true, true, _T("flags:\t") ).c_str()
				);

			info += d;
		}
	}

//////////////////////////////////////////////////////////////////////////

	if( arrEnvironment.size() > 0 )
	{
		d.Format( _T("==================================\n") );
		info += d;
		d.Format( _T("Process Environment:\n") );
		info += d;

		for( size_t i=0; i<arrEnvironment.size(); i++ )
		{
			tstring sEnv = arrEnvironment[i];
			d.Format( _T("\t%s\n"), sEnv.c_str() );
			info += d;
		}
	}

//////////////////////////////////////////////////////////////////////////

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

void CInformationDlg::OnPaint() 
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
			dc.DrawIcon( x, y, theApp.hMainIconSmall );
		}
	}
	else
	{
		CDialog::OnPaint();
	}
}

/////////////////////////////////////////////////////////////////////////////
