#if !defined(AFX_SYSTEMINFOLISTCTRL_H__CFA0E206_B822_4A36_91B8_55129F335FB5__INCLUDED_)
#define AFX_SYSTEMINFOLISTCTRL_H__CFA0E206_B822_4A36_91B8_55129F335FB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SystemInfoListCtrl.h : header file
//

class CSortClass;

/////////////////////////////////////////////////////////////////////////////
// SystemInfoListCtrl window

class SystemInfoListCtrl : public CListCtrl
{
// Construction
public:
	SystemInfoListCtrl();

// Attributes
public:

// Operations
public:
   BOOL SortTextItems( int nCol, BOOL bAscending, int low = 0, int high = -1 );
   BOOL GetSubItemText( int, LPCTSTR, CString& );
   BOOL GetSelectedSubItemText( int, CString& );
   BOOL GetSelectedSubItemText( LPCTSTR, CString& );
   int FindColumn( LPCTSTR );

protected:
   int nSortedCol; 
   BOOL bSortAscending;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SystemInfoListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SystemInfoListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(SystemInfoListCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg void OnHeaderClicked(NMHDR*, LRESULT* );

	DECLARE_MESSAGE_MAP()
};


// Sorter
class CSortClass
{
public:
	CSortClass(CListCtrl * _pWnd, const int _iCol, const bool _bIsNumeric);
	virtual ~CSortClass();
	
	int iCol;
	CListCtrl * pWnd;
	bool bIsNumeric;
	void Sort(const bool bAsc);
	
	static int CALLBACK CompareAsc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK CompareDes(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	
	static int CALLBACK CompareAscI(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK CompareDesI(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	
public:
	class CSortItem
	{
	public:
		virtual  ~CSortItem();
		CSortItem(const DWORD _dw, const CString &_txt);
		CString txt;
		DWORD dw;
	};
	class CSortItemInt
	{
	public:
		CSortItemInt(const DWORD _dw, const CString &_txt);
		int iInt ;
		DWORD dw;
	};
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSTEMINFOLISTCTRL_H__CFA0E206_B822_4A36_91B8_55129F335FB5__INCLUDED_)
