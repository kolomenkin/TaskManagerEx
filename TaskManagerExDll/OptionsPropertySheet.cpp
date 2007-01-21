// OptionsPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OptionsPropertySheet.h"
#include "TaskManagerExDll.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsPropertySheet

IMPLEMENT_DYNAMIC(COptionsPropertySheet, CPropertySheet)

COptionsPropertySheet::COptionsPropertySheet(CWnd* pWndParent)
	 : CPropertySheet(IDS_OPTIONS_PROPSHT_CAPTION, pWndParent)
{
	// Add all of the property pages here.  Note that
	// the order that they appear in here will be
	// the order they appear in on screen.  By default,
	// the first page of the set is the active one.
	// One way to make a different property page the 
	// active one is to call SetActivePage().

	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	AddPage(&m_PageGeneral);
	AddPage(&m_PageOther);
}

COptionsPropertySheet::~COptionsPropertySheet()
{
}


BEGIN_MESSAGE_MAP(COptionsPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(COptionsPropertySheet)
	ON_WM_CREATE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COptionsPropertySheet message handlers

int COptionsPropertySheet::DoModal() 
{
	BOOL oldShowSplash = theApp.m_options.GetShowSplash();
	BOOL oldShowTips = theApp.m_options.GetShowTips();
	COLORREF oldProcessColor = theApp.m_options.GetProcessColor();
	COLORREF oldServiceColor = theApp.m_options.GetServiceColor();
	COLORREF oldAlertColor = theApp.m_options.GetAlertColor();

	m_PageGeneral.m_bAutoRun = theApp.m_options.IsAutorun();
	m_PageGeneral.m_bSplash = oldShowSplash;
	m_PageGeneral.m_bTips = oldShowTips;
	m_PageOther.m_wndProcessColor.SetColor( oldProcessColor );
	m_PageOther.m_wndServiceColor.SetColor( oldServiceColor );
	m_PageOther.m_wndAlertColor.SetColor( oldAlertColor );
	
	int res = CPropertySheet::DoModal();
	if( res == IDOK )
	{
		COLORREF newProcessColor = m_PageOther.m_wndProcessColor.GetColor();
		COLORREF newServiceColor = m_PageOther.m_wndServiceColor.GetColor();
		COLORREF newAlertColor = m_PageOther.m_wndAlertColor.GetColor();
		BOOL newShowSplash = m_PageGeneral.m_bSplash;
		BOOL newShowTips = m_PageGeneral.m_bTips;

		theApp.m_options.SetAutorun( m_PageGeneral.m_bAutoRun );

		if( oldShowSplash != newShowSplash )
			theApp.m_options.SetShowSplash( newShowSplash );
		if( oldShowTips != newShowTips )
			theApp.m_options.SetShowTips( newShowTips );

		if( oldProcessColor != newProcessColor )
			theApp.m_options.SetProcessColor( newProcessColor );
		if( oldServiceColor != newServiceColor )
			theApp.m_options.SetServiceColor( newServiceColor );
		if( oldAlertColor != newAlertColor )
			theApp.m_options.SetAlertColor( newAlertColor );

		return res;
	}

	ASSERT( res == IDCANCEL );
	return res;
}

/////////////////////////////////////////////////////////////////////////////

int COptionsPropertySheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetIcon( theApp.hMainIconBig, TRUE );
	SetIcon( theApp.hMainIconSmall, FALSE );
//	SetClassLong( this->GetSafeHwnd(), GCL_HICON, (long)theApp.hMainIconBig );		// this changes icon for any dialog in the process!
//	SetClassLong( this->GetSafeHwnd(), GCL_HICONSM, (long)theApp.hMainIconSmall );	// this changes icon for any dialog in the process!

	return 0;
}

/////////////////////////////////////////////////////////////////////////////

void COptionsPropertySheet::OnPaint() 
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
		CPropertySheet::OnPaint();
	}
}
