#if !defined(AFX_SYSTEMINFODLG_H__A092E764_AF65_4E60_AF66_46AF2AC587BB__INCLUDED_)
#define AFX_SYSTEMINFODLG_H__A092E764_AF65_4E60_AF66_46AF2AC587BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SystemInfoDlg.h : header file
//

#include "resource.h"
#include "SystemInfoListCtrl.h"

class SystemInfoDlgThread : public CWinThread
{
protected:
	DECLARE_DYNCREATE( SystemInfoDlgThread )

public:
   virtual BOOL InitInstance();

public:
   BOOL Initialize( DWORD, WPARAM );
   static SystemInfoDlgThread* Start( DWORD, WPARAM );

public:
   DWORD    m_processID;
   WPARAM   m_command;
};

/////////////////////////////////////////////////////////////////////////////
// SystemInfoDlg dialog

class SystemInfoDlg : public CDialog
{
// Construction
public:
	SystemInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SystemInfoDlg)
	enum { IDD = IDD_SYSTEMINFODLG };
	SystemInfoListCtrl	m_SystemInfoList;
	//}}AFX_DATA

public:
   BOOL CreateToolbar();

   void RefreshAll();
   void RefreshList();
   void Resize();
   void ResizeToListCtrl();
   void ResizeColumns();

   ULONG GetSelectedHandle();
   ULONG GetSelectedHandle( int nCol );

   virtual void OnOK();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SystemInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  	CToolBar    m_wndToolBar;
  	CEdit       m_fileNameCtrl;
  	CStatic     m_fileNameStaticCtrl;

	// Generated message map functions
	//{{AFX_MSG(SystemInfoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysteminfodlgRefresh();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnHandlesRelease();
	afx_msg void OnModulesUnload();
	afx_msg void OnWindowsActivate();
	afx_msg void OnWindowsClose();
	afx_msg void OnHandlesEventSet();
	afx_msg void OnHandlesEventReset();
	afx_msg void OnHandlesEventPulse();
	afx_msg void OnHandlesMutantRelease();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSTEMINFODLG_H__A092E764_AF65_4E60_AF66_46AF2AC587BB__INCLUDED_)
