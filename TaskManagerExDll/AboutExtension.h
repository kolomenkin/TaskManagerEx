#if !defined(AFX_CAboutExtension_H__E0493685_3641_48FD_8855_2085BD5D5568__INCLUDED_)
#define AFX_CAboutExtension_H__E0493685_3641_48FD_8855_2085BD5D5568__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CAboutExtension.h : header file
//

#include "resource.h"
#include "TaskManagerExDllExport.h"

//class CAboutExtensionThread : public CWinThread
//{
//protected:
//	DECLARE_DYNCREATE( CAboutExtensionThread )
//
//public:
//   virtual BOOL InitInstance();
//   static TASKMANAGEREXDLL_DEBUG_API CAboutExtensionThread* Start();
//};

class CAboutExtensionThread
{
public:
	static TASKMANAGEREXDLL_DEBUG_API int Start( CWnd* pParent );
};

/////////////////////////////////////////////////////////////////////////////
// CAboutExtension dialog

class CAboutExtension : public CDialog
{
// Construction
public:
	CAboutExtension(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAboutExtension)
	enum { IDD = IDD_EXTENSION_ABOUT };
	CString	m_strVersion;
	CString	m_strComments;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutExtension)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual INT_PTR DoModal() override;
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrlMine m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CAboutExtension)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAboutExtension_H__E0493685_3641_48FD_8855_2085BD5D5568__INCLUDED_)
