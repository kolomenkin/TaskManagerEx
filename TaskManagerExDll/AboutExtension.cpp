// AboutExtension.cpp : implementation file
//

#include "stdafx.h"
#include "taskmanagerexdll.h"
#include "AboutExtension.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE( AboutExtensionThread, CWinThread )

BOOL AboutExtensionThread::InitInstance()
{
   AboutExtension dlg( CWnd::FromHandle( theApp.hwndTaskManager ) );
   m_pMainWnd = &dlg;

   dlg.DoModal();

   return FALSE;
}

AboutExtensionThread* AboutExtensionThread::Start()
{
   return (AboutExtensionThread*)::AfxBeginThread( RUNTIME_CLASS(AboutExtensionThread) );
}

/////////////////////////////////////////////////////////////////////////////
// AboutExtension dialog

AboutExtension::AboutExtension(CWnd* pParent /*=NULL*/)
	: CDialog(AboutExtension::IDD, pParent)
{
	//{{AFX_DATA_INIT(AboutExtension)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AboutExtension::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AboutExtension)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AboutExtension, CDialog)
	//{{AFX_MSG_MAP(AboutExtension)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AboutExtension message handlers
