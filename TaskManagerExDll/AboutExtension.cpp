// CAboutExtension.cpp : implementation file
//

#include "stdafx.h"
//#include "taskmanagerexdll.h"
#include "AboutExtension.h"
#include "DllVersion.h"
#include "Localization.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int CAboutExtensionThread::Start( CWnd* pParent )
{
	CAboutExtension dlg( pParent );
	INT_PTR res = dlg.DoModal();
	return res;
}

//IMPLEMENT_DYNCREATE( CAboutExtensionThread, CWinThread )
//
//BOOL CAboutExtensionThread::InitInstance()
//{
//	SetThreadNativeLanguage();
//	CAboutExtension dlg( AfxGetMainWnd() );
//	m_pMainWnd = &dlg;
//
//	dlg.DoModal();
//
//	PostQuitMessage( 0 ); // quit thread gracefully
//	return TRUE;
////	return FALSE; // return error to quit thread
//}
//
//CAboutExtensionThread* CAboutExtensionThread::Start()
//{
////	return (CAboutExtensionThread*)::AfxBeginThread( RUNTIME_CLASS(CAboutExtensionThread) );
//
//	// Always modal dialog:
//
//	SetThreadNativeLanguage();
//	CAboutExtension dlg( AfxGetMainWnd() );
//	dlg.DoModal();
//
//	return FALSE;
//}

/////////////////////////////////////////////////////////////////////////////
// CAboutExtension dialog

CAboutExtension::CAboutExtension(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutExtension::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutExtension)
	m_strVersion = _T("");
	m_strComments = _T("");
	//}}AFX_DATA_INIT

	TCHAR szTaskManagerExDll[MAX_PATH] = _T("");
	GetModuleFileName( AfxGetInstanceHandle(), szTaskManagerExDll, SIZEOF_ARRAY(szTaskManagerExDll) );
	CString version;

	GetModuleProductVersion( szTaskManagerExDll, version );

	/*
	// Usually version has the form "1, 2, 0, 0"...
	version.Replace( _T(','), _T('.') );
	version.Remove( _T(' ') );
	int pos;
	pos = version.Find( _T('.') );
	if( pos != -1 )
	{
		pos = version.Find( _T('.'), pos + 1 );
		if( pos != -1 )
		{
			version = version.Left(pos);
		}
	}
	*/

	m_strVersion.Format( _T("Task Manager Extension %s"), version );
	GetModuleComment( szTaskManagerExDll, m_strComments );
}


void CAboutExtension::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutExtension)
	DDX_Text(pDX, IDC_PRODUCT_VERSION_STATIC, m_strVersion);
	DDX_Text(pDX, IDC_COMMENTS_STATIC, m_strComments);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutExtension, CDialog)
	//{{AFX_MSG_MAP(CAboutExtension)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutExtension message handlers


BOOL CAboutExtension::PreTranslateMessage(MSG* pMsg) 
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CAboutExtension::OnHelpInfo(HELPINFO* pHelpInfo) 
{

	return CDialog::OnHelpInfo(pHelpInfo);
}

/////////////////////////////////////////////////////////////////////////////

BOOL CAboutExtension::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.Activate(TRUE);

		// TODO: Use one of the following forms to add controls:
		// m_tooltip.AddTool(GetDlgItem(IDC_<name>), <string-table-id>);
		// m_tooltip.AddTool(GetDlgItem(IDC_<name>), "<text>");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

INT_PTR CAboutExtension::DoModal()
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
