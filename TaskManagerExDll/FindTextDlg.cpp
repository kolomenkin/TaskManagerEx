// FindTextDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FindTextDlg.h"
#include "TaskManagerEx.h"
#include "AboutExtension.h"
#include "Localization.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define MAX_HISTORY_DEPTH	100

/////////////////////////////////////////////////////////////////////////////
// CFindTextDlg dialog


CFindTextDlg::CFindTextDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindTextDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindTextDlg)
	//}}AFX_DATA_INIT

	LoadHistory();
}


void CFindTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindTextDlg)
	DDX_Control(pDX, IDC_DIRECTION_DOWN_RADIO, m_wndDownRadio);
	DDX_Control(pDX, IDC_DIRECTION_UP_RADIO, m_wndUpRadio);
	DDX_Control(pDX, IDC_MATCH_CASE, m_wndMatchCase);
	DDX_Control(pDX, IDC_MATCH_WHOLE_WORD, m_wndMatchWholeWord);
	DDX_Control(pDX, IDC_TEXT_COMBO, m_wndTextCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindTextDlg, CDialog)
	//{{AFX_MSG_MAP(CFindTextDlg)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindTextDlg message handlers

void CFindTextDlg::OnOK() 
{
	m_wndTextCombo.GetWindowText( m_options.m_strFind );
	m_options.m_bMatchCase = m_wndMatchCase.GetCheck();
	m_options.m_bMatchWholeWordOnly = m_wndMatchWholeWord.GetCheck();
	m_options.m_bForward = !m_wndUpRadio.GetCheck();

	SaveHistory();

	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////

void CFindTextDlg::OnCancel() 
{
	CDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////

BOOL CFindTextDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_wndUpRadio.SetCheck( !m_options.m_bForward ? BST_CHECKED : BST_UNCHECKED );
	m_wndDownRadio.SetCheck( m_options.m_bForward ? BST_CHECKED : BST_UNCHECKED );
	m_wndMatchCase.SetCheck( m_options.m_bMatchCase ? BST_CHECKED : BST_UNCHECKED );
	m_wndMatchWholeWord.SetCheck( m_options.m_bMatchWholeWordOnly ? BST_CHECKED : BST_UNCHECKED );
	m_wndTextCombo.SetWindowText( m_options.m_strFind );
	for( int i=0; i<m_FindHistory.GetSize(); i++ )
	{
		m_wndTextCombo.InsertString( i, m_FindHistory[i] );
	}

	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.Activate(TRUE);

		m_tooltip.AddTool( GetDlgItem(IDC_TEXT_COMBO), LocLoadString(IDC_TEXT_COMBO) );
		m_tooltip.AddTool( GetDlgItem(IDC_MATCH_WHOLE_WORD), LocLoadString(IDC_MATCH_WHOLE_WORD) );
		m_tooltip.AddTool( GetDlgItem(IDC_MATCH_CASE), LocLoadString(IDC_MATCH_CASE) );
		m_tooltip.AddTool( GetDlgItem(IDC_DIRECTION_UP_RADIO), LocLoadString(IDC_DIRECTION_UP_RADIO) );
		m_tooltip.AddTool( GetDlgItem(IDC_DIRECTION_DOWN_RADIO), LocLoadString(IDC_DIRECTION_DOWN_RADIO) );
		m_tooltip.AddTool( GetDlgItem(IDOK), LocLoadString(IDS_FIND_OK) );
		m_tooltip.AddTool( GetDlgItem(IDCANCEL), LocLoadString(IDS_FIND_CANCEL) );
		

		// TODO: Use one of the following forms to add controls:
		// m_tooltip.AddTool(GetDlgItem(IDC_<name>), <string-table-id>);
		// m_tooltip.AddTool(GetDlgItem(IDC_<name>), "<text>");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

BOOL CFindTextDlg::LoadHistory()
{
	DWORD dwHistoryDepth = 0;

	HKEY hKey = NULL;
	DWORD dwType = REG_DWORD;
	DWORD dwSize = sizeof(DWORD);

	ASSERT( m_FindHistory.GetSize() == 0 );

	CString key;
	key.Format( _T("%s\\Find"), REG_KEY );
	::RegOpenKey( REG_ROOT, key, &hKey );

	CString	strFind;
	BOOL	bMatchCase = FALSE;
	BOOL	bMatchWholeWordOnly = FALSE;
	BOOL	bForward = TRUE; // Down

	if( hKey != NULL )
	{
		dwType = REG_DWORD; dwSize = sizeof(DWORD);
		::RegQueryValueEx( hKey, _T("MatchCase"), NULL, &dwType, 
				   (LPBYTE)&bMatchCase, &dwSize );

		dwType = REG_DWORD; dwSize = sizeof(DWORD);
		::RegQueryValueEx( hKey, _T("MatchWholeWordOnly"), NULL, &dwType, 
				   (LPBYTE)&bMatchWholeWordOnly, &dwSize );

		dwType = REG_DWORD; dwSize = sizeof(DWORD);
		::RegQueryValueEx( hKey, _T("FindForward"), NULL, &dwType, 
				   (LPBYTE)&bForward, &dwSize );

		dwType = REG_DWORD; dwSize = sizeof(DWORD);
		::RegQueryValueEx( hKey, _T("HistoryDepth"), NULL, &dwType, 
				   (LPBYTE)&dwHistoryDepth, &dwSize );

		for( DWORD i=0; i<dwHistoryDepth && i<MAX_HISTORY_DEPTH; i++ )
		{
			TCHAR szBuffer[1024] = _T("");

			CString value;
			value.Format( _T("Text_%02d"), i+1 );

			dwType = REG_SZ; dwSize = sizeof(szBuffer);
			LONG lRes = ::RegQueryValueEx( hKey, value, NULL, &dwType, 
					   (LPBYTE)szBuffer, &dwSize );

			if( lRes == ERROR_SUCCESS )
			{
				m_FindHistory.Add( szBuffer );
			}
		}

		::RegCloseKey( hKey );
	}

	if( m_FindHistory.GetSize() > 0 )
	{
		strFind = m_FindHistory[0];
	}

	m_options.m_strFind = strFind;
	m_options.m_bMatchCase = bMatchCase;
	m_options.m_bMatchWholeWordOnly = bMatchWholeWordOnly;
	m_options.m_bForward = bForward;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CFindTextDlg::SaveHistory()
{
	HKEY hKey = NULL;

	size_t i = 0;
	for( i=0; i<m_FindHistory.GetSize(); i++ )
	{
		CString s = m_FindHistory.GetAt( i );
		if( m_options.m_strFind == s )
		{
			m_FindHistory.RemoveAt( i );
			i--;
		}
	}
	m_FindHistory.InsertAt( 0, m_options.m_strFind );

	CString key;
	key.Format( _T("%s\\Find"), REG_KEY );
	::RegCreateKey( REG_ROOT, key, &hKey );

	if( hKey != NULL )
	{
		::RegSetValueEx( hKey, _T("MatchCase"), NULL, REG_DWORD, 
				   (const BYTE*)&m_options.m_bMatchCase, sizeof(DWORD) );

		::RegSetValueEx( hKey, _T("MatchWholeWordOnly"), NULL, REG_DWORD, 
				   (const BYTE*)&m_options.m_bMatchWholeWordOnly, sizeof(DWORD) );

		::RegSetValueEx( hKey, _T("FindForward"), NULL, REG_DWORD, 
				   (const BYTE*)&m_options.m_bForward, sizeof(DWORD) );

		size_t n = m_FindHistory.GetSize();

		for( i=0; i<n && i<MAX_HISTORY_DEPTH; i++ )
		{
			CString s = m_FindHistory.GetAt( i );
			CString value;
			value.Format( _T("Text_%02Id"), i+1 );

			::RegSetValueEx( hKey, value, NULL, REG_SZ,
					   (const BYTE*)(LPCTSTR)s, (s.GetLength()+1)*sizeof(TCHAR) );
		}

		DWORD dwRegVal = (DWORD) n;
		::RegSetValueEx( hKey, _T("HistoryDepth"), NULL, REG_DWORD,
			(const BYTE*)&dwRegVal, sizeof(DWORD));

		::RegCloseKey( hKey );
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CFindTextDlg::PreTranslateMessage(MSG* pMsg) 
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}

	return CDialog::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////

BOOL CFindTextDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	pHelpInfo = pHelpInfo;
	//CAboutExtensionThread::Start();
	return TRUE;
	//return CDialog::OnHelpInfo(pHelpInfo);
}

/////////////////////////////////////////////////////////////////////////////

INT_PTR CFindTextDlg::DoModal()
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
