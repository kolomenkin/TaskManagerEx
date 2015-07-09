// OptionsPropertySheet.h : header file
//
// This class defines custom modal property sheet 
// COptionsPropertySheet.
 
#ifndef __OPTIONSPROPERTYSHEET_H__
#define __OPTIONSPROPERTYSHEET_H__

#include "OptionsPropertyPages.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsPropertySheet

class COptionsPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(COptionsPropertySheet)

// Construction
public:
	COptionsPropertySheet(CWnd* pWndParent = NULL);

// Attributes
public:
	COptionsPropertyPageGeneral	m_PageGeneral;
	COptionsPropertyPageOther	m_PageOther;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsPropertySheet)
public:
	virtual INT_PTR DoModal() override;
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COptionsPropertySheet();

// Generated message map functions
protected:
	//{{AFX_MSG(COptionsPropertySheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __OPTIONSPROPERTYSHEET_H__
