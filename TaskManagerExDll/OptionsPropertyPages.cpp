// OptionsPropertyPages.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OptionsPropertyPages.h"
#include "Localization.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(COptionsPropertyPageGeneral, CPropertyPage)
IMPLEMENT_DYNCREATE(COptionsPropertyPageOther, CPropertyPage)


/////////////////////////////////////////////////////////////////////////////
// COptionsPropertyPageGeneral property page

COptionsPropertyPageGeneral::COptionsPropertyPageGeneral() : CPropertyPage(COptionsPropertyPageGeneral::IDD)
{
	//{{AFX_DATA_INIT(COptionsPropertyPageGeneral)
	m_bAutoRun = FALSE;
	m_bSplash = FALSE;
	m_bTips = FALSE;
	//}}AFX_DATA_INIT
}

COptionsPropertyPageGeneral::~COptionsPropertyPageGeneral()
{
}

void COptionsPropertyPageGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsPropertyPageGeneral)
	DDX_Check(pDX, IDC_AUTORUN_CHECK, m_bAutoRun);
	DDX_Check(pDX, IDC_SHOW_SPLASH, m_bSplash);
	DDX_Check(pDX, IDC_SHOW_TIPS, m_bTips);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsPropertyPageGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(COptionsPropertyPageGeneral)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COptionsPropertyPageOther property page

COptionsPropertyPageOther::COptionsPropertyPageOther() : CPropertyPage(COptionsPropertyPageOther::IDD)
{
	//{{AFX_DATA_INIT(COptionsPropertyPageOther)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

COptionsPropertyPageOther::~COptionsPropertyPageOther()
{
}

void COptionsPropertyPageOther::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsPropertyPageOther)
	DDX_Control(pDX, IDC_CPU_ALERT_COLOR, m_wndAlertColor);
	DDX_Control(pDX, IDC_SERVICE_PROCESS_COLOR, m_wndServiceColor);
	DDX_Control(pDX, IDC_PROCESS_COLOR, m_wndProcessColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsPropertyPageOther, CPropertyPage)
	//{{AFX_MSG_MAP(COptionsPropertyPageOther)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL COptionsPropertyPageGeneral::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.Activate(TRUE);

		m_tooltip.AddTool( GetDlgItem(IDC_AUTORUN_CHECK), LocLoadString(IDC_AUTORUN_CHECK) );
		m_tooltip.AddTool( GetDlgItem(IDC_SHOW_SPLASH), LocLoadString(IDC_SHOW_SPLASH) );
		m_tooltip.AddTool( GetDlgItem(IDC_SHOW_TIPS), LocLoadString(IDC_SHOW_TIPS) );

		// TODO: Use one of the following forms to add controls:
		// m_tooltip.AddTool(GetDlgItem(IDC_<name>), <string-table-id>);
		// m_tooltip.AddTool(GetDlgItem(IDC_<name>), "<text>");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsPropertyPageGeneral::OnDestroy() 
{
	CPropertyPage::OnDestroy();

}

BOOL COptionsPropertyPageGeneral::PreTranslateMessage(MSG* pMsg) 
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}

	return CPropertyPage::PreTranslateMessage(pMsg);
}

BOOL COptionsPropertyPageOther::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.Activate(TRUE);

		m_tooltip.AddTool( GetDlgItem(IDC_PROCESS_COLOR), LocLoadString(IDC_PROCESS_COLOR) );
		m_tooltip.AddTool( GetDlgItem(IDC_SERVICE_PROCESS_COLOR), LocLoadString(IDC_SERVICE_PROCESS_COLOR) );
		m_tooltip.AddTool( GetDlgItem(IDC_CPU_ALERT_COLOR), LocLoadString(IDC_CPU_ALERT_COLOR) );

		// TODO: Use one of the following forms to add controls:
		// m_tooltip.AddTool(GetDlgItem(IDC_<name>), <string-table-id>);
		// m_tooltip.AddTool(GetDlgItem(IDC_<name>), "<text>");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsPropertyPageOther::OnDestroy() 
{
	CPropertyPage::OnDestroy();

}

BOOL COptionsPropertyPageOther::PreTranslateMessage(MSG* pMsg) 
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}

	return CPropertyPage::PreTranslateMessage(pMsg);
}
