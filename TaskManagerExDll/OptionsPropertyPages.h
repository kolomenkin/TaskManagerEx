// OptionsPropertyPages.h : header file
//

#ifndef __OPTIONSPROPERTYPAGES_H__
#define __OPTIONSPROPERTYPAGES_H__

/////////////////////////////////////////////////////////////////////////////

#include "ColorBox.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsPropertyPageGeneral dialog

class COptionsPropertyPageGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(COptionsPropertyPageGeneral)

// Construction
public:
	COptionsPropertyPageGeneral();
	~COptionsPropertyPageGeneral();

// Dialog Data
	//{{AFX_DATA(COptionsPropertyPageGeneral)
	enum { IDD = IDD_OPTIONS_PROPPAGE_GENERAL };
	BOOL	m_bAutoRun;
	BOOL	m_bSplash;
	BOOL	m_bTips;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptionsPropertyPageGeneral)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrlMine m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(COptionsPropertyPageGeneral)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// COptionsPropertyPageOther dialog

class COptionsPropertyPageOther : public CPropertyPage
{
	DECLARE_DYNCREATE(COptionsPropertyPageOther)

// Construction
public:
	COptionsPropertyPageOther();
	~COptionsPropertyPageOther();

// Dialog Data
	//{{AFX_DATA(COptionsPropertyPageOther)
	enum { IDD = IDD_OPTIONS_PROPPAGE_OTHER };
	CColorBox	m_wndAlertColor;
	CColorBox	m_wndServiceColor;
	CColorBox	m_wndProcessColor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptionsPropertyPageOther)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrlMine m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(COptionsPropertyPageOther)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};



#endif // __OPTIONSPROPERTYPAGES_H__
