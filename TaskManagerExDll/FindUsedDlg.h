#if !defined(AFX_FINDUSEDDLG_H__A092E764_AF65_4E60_AF66_46AF2AC587BB__INCLUDED_)
#define AFX_FINDUSEDDLG_H__A092E764_AF65_4E60_AF66_46AF2AC587BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindUsedDlg.h : header file
//

#include "resource.h"
#include "SystemInfoListCtrl.h"

class FindUsedDlgThread : public CWinThread
{
protected:
	DECLARE_DYNCREATE( FindUsedDlgThread )

public:
   virtual BOOL InitInstance();

public:
   BOOL Initialize( WPARAM );
   static FindUsedDlgThread* Start( WPARAM );

public:
   WPARAM   m_command;
};

/////////////////////////////////////////////////////////////////////////////
// FindUsedDlg dialog

class FindUsedDlg : public CDialog
{
// Construction
public:
	FindUsedDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(FindUsedDlg)
	enum { IDD = IDD_FINDUSEDDLG };
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

   BOOL FreeSearchInString( LPCTSTR, LPCTSTR );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FindUsedDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  	CToolBar    m_wndToolBar;
  	CEdit       m_fileNameCtrl;
  	CStatic     m_fileNameStaticCtrl;

	// Generated message map functions
	//{{AFX_MSG(FindUsedDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnHandlesRelease();
	afx_msg void OnModulesUnload();
	afx_msg void OnExtensionRefresh();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDUSEDDLG_H__A092E764_AF65_4E60_AF66_46AF2AC587BB__INCLUDED_)
