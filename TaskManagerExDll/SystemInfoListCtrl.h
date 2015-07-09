#if !defined(AFX_CSystemInfoListCtrl_H__CFA0E206_B822_4A36_91B8_55129F335FB5__INCLUDED_)
#define AFX_CSystemInfoListCtrl_H__CFA0E206_B822_4A36_91B8_55129F335FB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CSystemInfoListCtrl.h : header file
//

class CSortClass;

/////////////////////////////////////////////////////////////////////////////

#define MINIMAL_COLUMN_WIDTH	30
#define DEFAULT_COLUMN_WIDTH	60

/////////////////////////////////////////////////////////////////////////////

#define LIST_IMAGE_WIDTH		1

/////////////////////////////////////////////////////////////////////////////
// CSystemInfoListCtrl window

class CSystemInfoListCtrl : public CListCtrl
{
// Construction
public:
	CSystemInfoListCtrl();

// Attributes
public:

// Operations
public:
	//BOOL SortTextItems( int nCol, BOOL bAscending, int low = 0, int high = -1 );
	BOOL GetSubItemText( int, LPCTSTR, CString& );
	int GetSelectedItem();
	void SetSelectedItem( int nItem );
	BOOL GetSelectedSubItemText( int, CString& );
	BOOL GetSelectedSubItemText( LPCTSTR, CString& );
	int FindColumn( LPCTSTR );

	void ReSort();

	//////////////////////////////////////////////////////////////
	//  Sorting:

	void SetSortedColumn(int nCol)
	{
		m_wndHeader.SetSortedColumn( nCol );
	}

	void SetSortAscending(BOOL bAscending)
	{
		m_wndHeader.SetSortAscending( bAscending );
	}

	BOOL IsSortAscending() const
	{
		BOOL res = m_wndHeader.IsSortAscending();
		return res;
	}
	int GetSortedColumn() const
	{
		int res = m_wndHeader.GetSortedColumn();
		return res;
	}

	int InsertColumn (int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT,
		int nWidth = DEFAULT_COLUMN_WIDTH, int nSubItem = -1)
	{
		int res = CListCtrl::InsertColumn( nCol, lpszColumnHeading, nFormat, nWidth, nSubItem );
		return res;
	}

protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSystemInfoListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSystemInfoListCtrl();

	// Generated message map functions
protected:

	////////////////////////////////////////////////////////////////////////////////
	// The header control only used by CSystemInfoListCtrl
	////////////////////////////////////////////////////////////////////////////////
	class CReportHeaderCtrl : public CHeaderCtrl			
	{
	public:		
		CReportHeaderCtrl();
		virtual ~CReportHeaderCtrl() {};	
		void SetSortedColumn(int nCol);
		void SetSortAscending(BOOL bAscending);
		BOOL IsSortAscending() const;
		int GetSortedColumn() const;
		void UpdateSortArrow();

	protected:
		void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
		int m_iSortColumn;
		BOOL m_bSortAscending;
	};

	// Member data
	CReportHeaderCtrl m_wndHeader;
	
	//{{AFX_MSG(CSystemInfoListCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
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
		CSortItem(DWORD_PTR _dw, int indent, const CString &_txt);
		CString	txt;	// item text
		int		indent;	// item indent
		DWORD_PTR	dw;		// old user item User Data
	};
	class CSortItemInt
	{
	public:
		CSortItemInt(DWORD_PTR _dw, int indent, const CString &_txt);
		int		iInt;	// item value
		int		indent;	// item indent
		DWORD_PTR	dw;		// old user item User Data
	};
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CSystemInfoListCtrl_H__CFA0E206_B822_4A36_91B8_55129F335FB5__INCLUDED_)
