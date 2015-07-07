#if !defined(AFX_CSystemInfoDlg_H__A092E764_AF65_4E60_AF66_46AF2AC587BB__INCLUDED_)
#define AFX_CSystemInfoDlg_H__A092E764_AF65_4E60_AF66_46AF2AC587BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CSystemInfoDlg.h : header file
//

#include "resource.h"
#include "SystemInfoListCtrl.h"
#include "FindTextDlg.h"
#include "TaskManagerExDllExport.h"

/////////////////////////////////////////////////////////////////////////////

class CSystemInfoDlgThread : public CWinThread
{
protected:
	DECLARE_DYNCREATE( CSystemInfoDlgThread )

public:
	virtual BOOL InitInstance() override;

public:
	static TASKMANAGEREXDLL_DEBUG_API CSystemInfoDlgThread* Start( DWORD pID, WPARAM wCommand, BOOL bFind, BOOL bMultiProcess );
	
public:

	struct INIT_SYSTEM_INFO
	{
		DWORD		m_processID;
		WPARAM		m_command;
		BOOL		m_bFind;
		BOOL		m_bMultiProcess;
	};

	INIT_SYSTEM_INFO	initInfo;
};

/////////////////////////////////////////////////////////////////////////////
// CSystemInfoDlg dialog

class CSystemInfoDlg : public CDialog
{
// Construction
public:
	CSystemInfoDlg(CWnd* pParent = NULL);   // standard constructor
	~CSystemInfoDlg();

// Dialog Data
	//{{AFX_DATA(CSystemInfoDlg)
	enum { IDD = IDD_SYSTEMINFODLG };
	CSystemInfoListCtrl	m_SystemInfoList;
	//}}AFX_DATA

	CSystemInfoDlgThread::INIT_SYSTEM_INFO	initInfo;

	WPARAM GetInitCommand() const
	{
		return initInfo.m_command;
	}

	BOOL IsInitFind() const
	{
		return initInfo.m_bFind;
	}

	BOOL IsInitMultiprocess() const
	{
		return initInfo.m_bMultiProcess;
	}

	DWORD GetInitProcessId() const
	{
		return initInfo.m_processID;
	}


public:
	BOOL CreateToolbar();
	BOOL CreateStatusBar();

	void RefreshAll();
	void RefreshList();
	void RefreshStatus();
	void Resize();
	void ResizeToListCtrl();
	void ResizeColumns();

	HANDLE GetSelectedHandle();

	DWORD GetSelectedPID();
	
	void FindTextOnce( BOOL bLookForward );

	virtual void OnOK();
	virtual void OnCancel();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSystemInfoDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrlMine m_tooltip;
	HACCEL		m_hAccelTable;
	CToolBar	m_wndToolBar;
	CStatusBar	m_wndStatusBar;
	CEdit		m_fileNameCtrl;
	CStatic		m_fileNameStaticCtrl;

	FIND_OPTIONS	m_FindOptions;

	BOOL	m_bFirstFind;
	BOOL	m_bResizeListDuringRefresh;
	afx_msg void OnListUpdate();

	CImageList	m_ImageList;
	BOOL		m_bExpandRegions; // in Memory Map mode

	// Generated message map functions
	//{{AFX_MSG(CSystemInfoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExtensionRefresh();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnHandlesRelease();
	afx_msg void OnModulesUnload();
	afx_msg void OnWindowsClose();
	afx_msg void OnHandlesEventSet();
	afx_msg void OnHandlesEventReset();
	afx_msg void OnHandlesEventPulse();
	afx_msg void OnHandlesMutantRelease();
	afx_msg void OnModulesLoad();
	afx_msg void OnHandlesProcessExitprocess();
	afx_msg void OnHandlesProcessTerminate();
	afx_msg void OnHandlesThreadResume();
	afx_msg void OnHandlesThreadSuspend();
	afx_msg void OnHandlesThreadTerminate();
	afx_msg void OnHandlesWindowstationAssigntoprocess();
	afx_msg void OnHandlesDesktopSwithto();
	afx_msg void OnModulesFileproperties();
	afx_msg void OnModulesDependency();
	afx_msg void OnItemchangedListview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFindText();
	afx_msg void OnFindNextText();
	afx_msg void OnFindPrevText();
	afx_msg void OnMemoryExpand();
	afx_msg void OnMemoryCompact();
	afx_msg void OnDestroy();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnPaint();
	afx_msg void OnHandlesSecurity();
	afx_msg void OnModulesSecurity();
	afx_msg void OnHandlesThreadSecurity();
	afx_msg void OnClose();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg BOOL OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CSystemInfoDlg_H__A092E764_AF65_4E60_AF66_46AF2AC587BB__INCLUDED_)
