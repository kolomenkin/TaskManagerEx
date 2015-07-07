#if !defined(AFX_PROCESSINFODLG_H__729E084F_52EA_4580_BE34_39A14720801A__INCLUDED_)
#define AFX_PROCESSINFODLG_H__729E084F_52EA_4580_BE34_39A14720801A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcessInfoDlg.h : header file
//

#include "resource.h"
#include "TaskManagerExDllExport.h"
#include "WindowsCore.h"

/////////////////////////////////////////////////////////////////////////////

#pragma warning( push, 3 )
#pragma warning( disable: 4786 )
#	include <string>
#	include <vector>
#pragma  warning(pop)

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

/////////////////////////////////////////////////////////////////////////////

class CInformationDlgThread : public CWinThread
{
protected:
	DECLARE_DYNCREATE( CInformationDlgThread )

public:
	virtual BOOL InitInstance() override;

public:
	BOOL Initialize( DWORD pID, WPARAM command );
	static TASKMANAGEREXDLL_DEBUG_API CInformationDlgThread* Start( DWORD pID, WPARAM command );

public:
	DWORD	m_processID;
	WPARAM	m_command;
};

/////////////////////////////////////////////////////////////////////////////
// CInformationDlg dialog

class CInformationDlg : public CDialog, public IAdvapi32
{
// Construction
public:
	CInformationDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInformationDlg)
	enum { IDD = IDD_INFORMATION };
	CButton	m_wndExit;
	CEdit	m_wndInfo;
	//}}AFX_DATA

	CFont	m_Font;

	BOOL GetProcessInformation( CString& info );
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInformationDlg)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual INT_PTR DoModal() override;
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrlMine	m_tooltip;
	CSize			m_OldClientSize;

	struct MY_SERVICE_INFO
	{
		tstring sName;
		tstring sDisplay;
		SERVICE_STATUS_PROCESS ServiceStatusProcess;
	};
	
	// Generated message map functions
	//{{AFX_MSG(CInformationDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual void OnOK() override;
	virtual void OnCancel() override;
	afx_msg void OnExit();
	afx_msg void OnRefresh();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg void OnGetMinMaxInfo( MINMAXINFO* pMinMaxInfo );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSINFODLG_H__729E084F_52EA_4580_BE34_39A14720801A__INCLUDED_)
