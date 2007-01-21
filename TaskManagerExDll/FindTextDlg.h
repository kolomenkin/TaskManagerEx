#if !defined(AFX_FINDTEXTDLG_H__2B22D15D_36DB_4AC9_BEBC_A418F0B48069__INCLUDED_)
#define AFX_FINDTEXTDLG_H__2B22D15D_36DB_4AC9_BEBC_A418F0B48069__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindTextDlg.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////

struct FIND_OPTIONS
{
	CString			m_strFind;
	BOOL			m_bMatchCase;
	BOOL			m_bMatchWholeWordOnly;
	BOOL			m_bForward; // Down

	FIND_OPTIONS()
	{
		m_bMatchCase = FALSE;
		m_bMatchWholeWordOnly = FALSE;
		m_bForward = TRUE;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CFindTextDlg dialog

class CFindTextDlg : public CDialog
{
// Construction
public:
	CFindTextDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFindTextDlg)
	enum { IDD = IDD_FIND_TEXT };
	CButton	m_wndDownRadio;
	CButton	m_wndUpRadio;
	CButton	m_wndMatchCase;
	CButton	m_wndMatchWholeWord;
	CComboBox	m_wndTextCombo;
	//}}AFX_DATA

	CStringArray	m_FindHistory;
	FIND_OPTIONS	m_options;

	BOOL LoadHistory();
	BOOL SaveHistory();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindTextDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrlMine m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CFindTextDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDTEXTDLG_H__2B22D15D_36DB_4AC9_BEBC_A418F0B48069__INCLUDED_)
