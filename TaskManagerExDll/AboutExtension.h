#if !defined(AFX_ABOUTEXTENSION_H__E0493685_3641_48FD_8855_2085BD5D5568__INCLUDED_)
#define AFX_ABOUTEXTENSION_H__E0493685_3641_48FD_8855_2085BD5D5568__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AboutExtension.h : header file
//

#include "resource.h"

class AboutExtensionThread : public CWinThread
{
protected:
	DECLARE_DYNCREATE( AboutExtensionThread )

public:
   virtual BOOL InitInstance();
   static AboutExtensionThread* Start();
};

/////////////////////////////////////////////////////////////////////////////
// AboutExtension dialog

class AboutExtension : public CDialog
{
// Construction
public:
	AboutExtension(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AboutExtension)
	enum { IDD = IDD_EXTENSION_ABOUT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AboutExtension)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AboutExtension)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUTEXTENSION_H__E0493685_3641_48FD_8855_2085BD5D5568__INCLUDED_)
