#include "stdafx.h"
#include "resource.h"
// CG: This file added by 'Tip of the Day' component.

#include <winreg.h>
#include "TipDlg.h"
#include "Localization.h"
#include "TaskManagerExDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTipDlg dialog

CTipDlg::CTipDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TIP, pParent)
{
	//{{AFX_DATA_INIT(CTipDlg)
	m_bStartup = TRUE;
	//}}AFX_DATA_INIT

	m_bStartup = g_bStartTipsByDefault;

	// We need to find out what the startup and file position parameters are
	// If startup does not exist, we assume that the Tips on startup is checked TRUE.
	CWinApp* pApp = AfxGetApp();
	m_bStartup = pApp->GetProfileInt(g_szTipsSection, g_szTipsIntStartup, g_bStartTipsByDefault );
	UINT iFilePos = pApp->GetProfileInt(g_szTipsSection, g_szTipsIntFilePos, 0);

	m_strTips = LocLoadString( IDS_TIPS );
	m_iTipPos = iFilePos;

	{
		GetNextTipString(m_strTip);
	}
}

CTipDlg::~CTipDlg()
{
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileInt(g_szTipsSection, g_szTipsIntFilePos, m_iTipPos );
}
        
void CTipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTipDlg)
	DDX_Check(pDX, IDC_TIP_STARTUP, m_bStartup);
	DDX_Text(pDX, IDC_TIPSTRING, m_strTip);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTipDlg, CDialog)
	//{{AFX_MSG_MAP(CTipDlg)
	ON_BN_CLICKED(IDC_NEXTTIP, OnNextTip)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTipDlg message handlers

void CTipDlg::OnNextTip()
{
	GetNextTipString(m_strTip);
	UpdateData(FALSE);
}

void CTipDlg::GetNextTipString(CString& strNext)
{
	// This routine identifies the next string that needs to be
	// read from the tips file
	int iFoundEnd = 0;
	BOOL bStop = FALSE;
	while (!bStop) 
	{
		strNext.Empty();

		while( m_iTipPos < m_strTips.GetLength() )
		{
			TCHAR ch = m_strTips[m_iTipPos];
			strNext += ch;
			m_iTipPos++;

			if( ch == _T('\n') )
			{
				break;
			}
		}

		if( m_iTipPos >= m_strTips.GetLength() )
		{
			m_iTipPos = 0;
			iFoundEnd++;
		}

		if( iFoundEnd > 1 || m_strTips.GetLength()==0 ) // Found end twice - there is not tips!
		{
			GetDlgItem(IDC_NEXTTIP)->EnableWindow(FALSE);
			strNext = LocLoadString(IDS_TIPS_FILE_ABSENT);
			break;
		}

		if( !strNext.IsEmpty() )
		{
			TCHAR ch = strNext[0];
			if (ch != _T(' ')	&& ch != _T('\t') && 
				ch != _T('\n')	&& ch != _T(';') &&
				ch != _T('\0') ) 
			{
				// There should be no space at the beginning of the tip
				// This behavior is same as VC++ Tips file
				// Comment lines are ignored and they start with a semicolon
				bStop = TRUE;
			}
		}
	}
	strNext.ReleaseBuffer();
}

HBRUSH CTipDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (pWnd->GetDlgCtrlID() == IDC_TIPSTRING)
		return (HBRUSH)GetStockObject(WHITE_BRUSH);

	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CTipDlg::OnOK()
{
	CDialog::OnOK();

    // Update the startup information stored in the INI file
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileInt(g_szTipsSection, g_szTipsIntStartup, m_bStartup);
}

BOOL CTipDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon( theApp.hMainIconBig, TRUE );
	SetIcon( theApp.hMainIconSmall, FALSE );
//	SetClassLong( this->GetSafeHwnd(), GCL_HICON, (long)theApp.hMainIconBig );		// this changes icon for any dialog in the process!
//	SetClassLong( this->GetSafeHwnd(), GCL_HICONSM, (long)theApp.hMainIconSmall );	// this changes icon for any dialog in the process!

	// If Tips file does not exist then disable NextTip
	if( m_strTips.IsEmpty() )
		GetDlgItem(IDC_NEXTTIP)->EnableWindow(FALSE);

	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.Activate(TRUE);

		m_tooltip.AddTool( GetDlgItem(IDC_TIP_STARTUP), LocLoadString(IDC_TIP_STARTUP) );
		m_tooltip.AddTool( GetDlgItem(IDC_NEXTTIP), LocLoadString(IDC_NEXTTIP) );
		m_tooltip.AddTool( GetDlgItem(IDOK), LocLoadString(IDS_TIPS_OK) );

		// TODO: Use one of the following forms to add controls:
		// m_tooltip.AddTool(GetDlgItem(IDC_<name>), <string-table-id>);
		// m_tooltip.AddTool(GetDlgItem(IDC_<name>), "<text>");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CTipDlg::OnPaint()
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
		return;
	}

	CPaintDC dc(this); // device context for painting

	// Get paint area for the big static control
	CWnd* pStatic = GetDlgItem(IDC_FRAME_INTERNAL);
	CRect rect;
	pStatic->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Paint the background white.
	CBrush brush;
	brush.CreateStockObject(WHITE_BRUSH);
	dc.FillRect(rect, &brush);

	// Load bitmap and get dimensions of the bitmap
	CBitmap bmp;
	bmp.LoadBitmap(IDB_LIGHTBULB);
	BITMAP bmpInfo;
	ZeroMemory( &bmpInfo, sizeof(bmpInfo) );
	bmp.GetBitmap(&bmpInfo);

	// Draw bitmap in top corner and validate only top portion of window
	CDC dcTmp;
	dcTmp.CreateCompatibleDC(&dc);
	dcTmp.SelectObject(&bmp);
	rect.bottom = bmpInfo.bmHeight + rect.top;
	dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), 
		&dcTmp, 0, 0, SRCCOPY);

	// Draw out "Did you know..." message next to the bitmap
	CString strMessage;
	strMessage = LocLoadString(IDS_TIPS_DIDYOUKNOW);
	rect.left += bmpInfo.bmWidth;
	dc.SetBkMode( TRANSPARENT );
	dc.DrawText(strMessage, rect, DT_VCENTER | DT_SINGLELINE);

	// Do not call CDialog::OnPaint() for painting messages
}


INT_PTR CTipDlg::DoModal() 
{
	// load resource as necessary
	if( m_lpszTemplateName != NULL && m_lpDialogTemplate == NULL )
	{
		m_lpDialogTemplate = LocLoadDialog( m_lpszTemplateName );
		m_lpszTemplateName = NULL;
	}

	return CDialog::DoModal();
}

BOOL CTipDlg::PreTranslateMessage(MSG* pMsg) 
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}

	return CDialog::PreTranslateMessage(pMsg);
}
