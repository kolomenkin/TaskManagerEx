// CSystemInfoListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "taskmanagerexdll.h"
#include "SystemInfoListCtrl.h"
#include "resource.h"
#include "Localization.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char g_szStamp_Mark[] = "\r\n\r\nSystemInfoListCtrl.cpp Timestamp: " __DATE__ ", " __TIME__ "\r\n\r\n";

////////////////////////////////////////////////////////////////////////////
// CReportHeaderCtrl implementation
////////////////////////////////////////////////////////////////////////////
CSystemInfoListCtrl::CReportHeaderCtrl::CReportHeaderCtrl()
{
	m_iSortColumn = -1;
	m_bSortAscending = TRUE;
}

int CSystemInfoListCtrl::CReportHeaderCtrl::GetSortedColumn() const
{
	return m_iSortColumn;
}

BOOL CSystemInfoListCtrl::CReportHeaderCtrl::IsSortAscending() const
{
	return m_bSortAscending;
}

void CSystemInfoListCtrl::CReportHeaderCtrl::SetSortAscending(BOOL bAscending)
{
	m_bSortAscending = bAscending;
}

void CSystemInfoListCtrl::CReportHeaderCtrl::SetSortedColumn(int nCol)
{
	m_iSortColumn = nCol;
}

////////////////////////////////////////////////////////////////////////////////
// Mark Jackson's code
////////////////////////////////////////////////////////////////////////////////
void CSystemInfoListCtrl::CReportHeaderCtrl::UpdateSortArrow()
{
	// change the item to owner drawn.
	HD_ITEM hditem;

	if( m_iSortColumn != -1 )
	{
		hditem.mask = HDI_FORMAT;
		VERIFY(GetItem(m_iSortColumn, &hditem));
		hditem.fmt |= HDF_OWNERDRAW;
		VERIFY(SetItem(m_iSortColumn, &hditem));
	}

	// invalidate the header control so it gets redrawn
	Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
// Mark Jackson's code
////////////////////////////////////////////////////////////////////////////////
void CSystemInfoListCtrl::CReportHeaderCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// attath to the device context.
	CDC dc;
	VERIFY(dc.Attach(lpDrawItemStruct->hDC));

	// save the device context.
	const int iSavedDC = dc.SaveDC();

	// get the column rect.
	CRect rc(lpDrawItemStruct->rcItem);

	// set the clipping region to limit drawing within the column.
	CRgn rgn;
	VERIFY(rgn.CreateRectRgnIndirect(&rc));
	(void)dc.SelectObject(&rgn);
	VERIFY(rgn.DeleteObject());

	// draw the background,
	CBrush brush(GetSysColor(COLOR_3DFACE));
	dc.FillRect(rc, &brush);

	// get the column text and format.
	TCHAR szText[ 256 ];
	HD_ITEM hditem;

	hditem.mask = HDI_TEXT | HDI_FORMAT;
	hditem.pszText = szText;
	hditem.cchTextMax = 255;

	VERIFY(GetItem(lpDrawItemStruct->itemID, &hditem));

	// determine the format for drawing the column label.
	UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS ;

	if(hditem.fmt & HDF_CENTER)
		uFormat |= DT_CENTER;
	else if(hditem.fmt & HDF_RIGHT)
		uFormat |= DT_RIGHT;
	else
		uFormat |= DT_LEFT;

	// adjust the rect if the mouse button is pressed on it.
	if(lpDrawItemStruct->itemState == ODS_SELECTED)
	{
		rc.left++;
		rc.top += 2;
		rc.right++;
	}

	CRect rcIcon(lpDrawItemStruct->rcItem);
	const int iOffset = (rcIcon.bottom - rcIcon.top) / 4;

	// adjust the rect further if the sort arrow is to be displayed.
	if(lpDrawItemStruct->itemID == (UINT)m_iSortColumn)
		rc.right -= 3 * iOffset;

	rc.left += iOffset;
	rc.right -= iOffset;

	// draw the column label.
	if(rc.left < rc.right)
		(void)dc.DrawText(szText, -1, rc, uFormat);

	// draw the sort arrow.
	if(lpDrawItemStruct->itemID == (UINT)m_iSortColumn)
	{
		// set up the pens to use for drawing the arrow.
		CPen penLight(PS_SOLID, 1, GetSysColor(COLOR_3DHILIGHT));
		CPen penShadow(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
		CPen* pOldPen = dc.SelectObject(&penLight);

		if(m_bSortAscending)
		{
			// draw the arrow pointing upwards.
			dc.MoveTo(rcIcon.right - 2 * iOffset, iOffset);
			dc.LineTo(rcIcon.right - iOffset, rcIcon.bottom - iOffset - 1);
			dc.LineTo(rcIcon.right - 3 * iOffset - 2, rcIcon.bottom - iOffset - 1);
			(void)dc.SelectObject(&penShadow);
			dc.MoveTo(rcIcon.right - 3 * iOffset - 1, rcIcon.bottom - iOffset - 1);
			dc.LineTo(rcIcon.right - 2 * iOffset, iOffset - 1);		
		}
		else
		{
			// draw the arrow pointing downwards.
			dc.MoveTo(rcIcon.right - iOffset - 1, iOffset);
			dc.LineTo(rcIcon.right - 2 * iOffset - 1, rcIcon.bottom - iOffset);
			(void)dc.SelectObject(&penShadow);
			dc.MoveTo(rcIcon.right - 2 * iOffset - 2, rcIcon.bottom - iOffset);
			dc.LineTo(rcIcon.right - 3 * iOffset - 1, iOffset);
			dc.LineTo(rcIcon.right - iOffset - 1, iOffset);		
		}

		// restore the pen.
		(void)dc.SelectObject(pOldPen);
	}

	// restore the previous device context.
	VERIFY(dc.RestoreDC(iSavedDC));

	// detach the device context before returning.
	(void)dc.Detach();
}


////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSystemInfoListCtrl implementation
////////////////////////////////////////////////////////////////////////////

CSystemInfoListCtrl::CSystemInfoListCtrl()
{
	SetSortedColumn( -1 );
	SetSortAscending( TRUE );
}

CSystemInfoListCtrl::~CSystemInfoListCtrl()
{
}


BEGIN_MESSAGE_MAP(CSystemInfoListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CSystemInfoListCtrl)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_NOTIFY(HDN_ITEMCLICKA, 0, OnHeaderClicked) 
	ON_NOTIFY(HDN_ITEMCLICKW, 0, OnHeaderClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemInfoListCtrl message handlers

int CSystemInfoListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CSystemInfoListCtrl::PreSubclassWindow() 
{
	// the list control must have the report style.
	ASSERT(GetStyle() & LVS_REPORT);

	CListCtrl::PreSubclassWindow();
	VERIFY(m_wndHeader.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd()));

	ModifyStyle( 0,
		LVS_REPORT |
		LVS_SHOWSELALWAYS |
//		LVS_SINGLESEL |
		0
		);

	SetExtendedStyle( GetExtendedStyle() |
		LVS_EX_FULLROWSELECT |
		LVS_EX_HEADERDRAGDROP |
		LVS_EX_GRIDLINES |
		LVS_EX_INFOTIP |
//		LVS_EX_ONECLICKACTIVATE |
//		LVS_EX_UNDERLINEHOT |
//		LVS_EX_TRACKSELECT |
//		LVS_EX_FLATSB |
		0
		);

	CListCtrl::PreSubclassWindow();
}

void CSystemInfoListCtrl::ReSort()
{
	HDITEM hi;
	::ZeroMemory( &hi, sizeof(hi) );
	hi.mask = HDI_FORMAT;
	int iCol = GetSortedColumn();
	BOOL bAscending = IsSortAscending();
	m_wndHeader.GetItem( iCol, &hi );

	CSortClass csc(this, iCol, (hi.fmt & LVCFMT_RIGHT)!=0 );

	// show the appropriate arrow in the header control.
	m_wndHeader.UpdateSortArrow();

	csc.Sort( bAscending == TRUE );
}

void CSystemInfoListCtrl::OnHeaderClicked(NMHDR* pNMHDR, LRESULT* pResult) 
{
   HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;

   if( phdn->iButton == 0 )
   {
		// User clicked on header using left mouse button
		int iCol = GetSortedColumn();
		BOOL bAscending = IsSortAscending();

		if( phdn->iItem == iCol )
			SetSortAscending( !bAscending );
		else
			SetSortAscending( TRUE );

		SetSortedColumn( phdn->iItem );

		ReSort();
   }

	*pResult = 0;
}

int CSystemInfoListCtrl::FindColumn( LPCTSTR lpszColumn )
{
   int nCol = -1;
   HDITEM headerInfo;
   TCHAR  lpBuffer[256];

   headerInfo.mask = HDI_TEXT;
   headerInfo.pszText = lpBuffer;
   headerInfo.cchTextMax = 256;

   for ( int i = 0; i < m_wndHeader.GetItemCount(); i++ )
   {
      m_wndHeader.GetItem( i, &headerInfo );

      if ( _tcscmp( lpszColumn, headerInfo.pszText ) == 0 )
         return i;
   }

   return nCol;   
}

BOOL CSystemInfoListCtrl::GetSubItemText( int nPos, LPCTSTR lpszColumn, CString& subItemText )
{
   int nCol = FindColumn( lpszColumn );

   if ( nCol == -1 )
      return FALSE;

   subItemText = GetItemText( nPos, nCol );

   return TRUE;
}

BOOL CSystemInfoListCtrl::GetSelectedSubItemText( int nCol, CString& subItemText )
{
   POSITION pos = GetFirstSelectedItemPosition();

   if ( pos == NULL )
      return FALSE;

   subItemText = GetItemText( GetNextSelectedItem( pos ), nCol );

   return pos == NULL;
}

int CSystemInfoListCtrl::GetSelectedItem()
{
	POSITION pos = GetFirstSelectedItemPosition();

	if ( pos == NULL )
		return -1;

	int iItem = GetNextSelectedItem( pos );

	return iItem;
}

void CSystemInfoListCtrl::SetSelectedItem( int nItem )
{
	int iLastSelected = GetSelectedItem();
	if( iLastSelected == nItem )
	{
		return;
	}

	if( iLastSelected != -1 )
	{
		SetItemState( iLastSelected, 0, LVIS_SELECTED );
	}

	if( nItem != -1 )
	{
		SetItemState( nItem, LVIS_SELECTED, LVIS_SELECTED );
	}
}

BOOL CSystemInfoListCtrl::GetSelectedSubItemText( LPCTSTR lpszColumn, CString& subItemText )
{
   int nCol = FindColumn( lpszColumn );

   if ( nCol == -1 )
      return FALSE;

   return GetSelectedSubItemText( nCol, subItemText );
}

void CSystemInfoListCtrl::OnContextMenu(CWnd* pWnd, CPoint) 
{
   GetParent()->SendMessage( WM_CONTEXTMENU, (WPARAM)pWnd->GetSafeHwnd() );
}

void CSystemInfoListCtrl::OnPaint() 
{
	CListCtrl::OnPaint();
	//Default();

	if (GetItemCount() <= 0)
	{
		COLORREF clrText = ::GetSysColor(COLOR_WINDOWTEXT);
		COLORREF clrTextBk = ::GetSysColor(COLOR_WINDOW);

		CDC* pDC = GetDC();
		// Save dc state
		int nSavedDC = pDC->SaveDC();

		CRect rc;
		GetWindowRect(&rc);
		ScreenToClient(&rc);

		CHeaderCtrl* pHC;
		pHC = &m_wndHeader;
		if (pHC != NULL)
		{
			CRect rcH;
			pHC->GetItemRect(0, &rcH);
			rc.top += rcH.bottom;
		}
		rc.top += 10;

		CString strText;
		strText = LocLoadString( IDS_LIST_NO_ITEMS );
		CBrush brushTextBk(clrTextBk);

		pDC->SetTextColor(clrText);
		pDC->SetBkColor(clrTextBk);
		pDC->FillRect(rc, &brushTextBk);
		pDC->SelectStockObject(ANSI_VAR_FONT);
		pDC->DrawText(strText, -1, rc, 
					  DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP);

		// Restore dc
		pDC->RestoreDC(nSavedDC);
		ReleaseDC(pDC);
	}
	// Do not call CListCtrl::OnPaint() for painting messages
}


/////////////////////////////////////////////////////////////////////////////
// CSortClass

CSortClass::CSortClass(CListCtrl * _pWnd, const int _iCol, const bool _bIsNumeric)
{
	iCol = _iCol;
	pWnd = _pWnd;
	bIsNumeric = _bIsNumeric;
	
	ASSERT(pWnd);
	int max = pWnd->GetItemCount();
	DWORD_PTR dw = 0;
	CString txt;
	if (bIsNumeric)
	{
		for (int t = 0; t < max; t++)
		{
			LVITEM item;
			item.iItem = t;
			item.iSubItem = 0;
			item.mask = LVIF_INDENT;
			item.iIndent = 0;
			pWnd->GetItem( &item );

			dw = pWnd->GetItemData(t);
			txt = pWnd->GetItemText(t, iCol);
			pWnd->SetItemData(t, (DWORD) new CSortItemInt(dw, item.iIndent, txt));
		}
	}
	else
	{
		for (int t = 0; t < max; t++)
		{
			LVITEM item;
			item.iItem = t;
			item.iSubItem = 0;
			item.mask = LVIF_INDENT;
			item.iIndent = 0;
			pWnd->GetItem( &item );

			dw = pWnd->GetItemData(t);
			txt = pWnd->GetItemText(t, iCol);
			pWnd->SetItemData(t, (DWORD) new CSortItem(dw, item.iIndent, txt));
		}
	}
}

CSortClass::~CSortClass()
{
	ASSERT(pWnd);
	int max = pWnd->GetItemCount();
	if (bIsNumeric)
	{
		CSortItemInt * pItem;
		for (int t = 0; t < max; t++)
		{
			pItem = (CSortItemInt *) pWnd->GetItemData(t);
			ASSERT(pItem);
			pWnd->SetItemData(t, pItem->dw);
			delete pItem;
		}
	}
	else
	{
		CSortItem * pItem;
		for (int t = 0; t < max; t++)
		{
			pItem = (CSortItem *) pWnd->GetItemData(t);
			ASSERT(pItem);
			pWnd->SetItemData(t, pItem->dw);
			delete pItem;
		}
	}
}

void
CSortClass::Sort(const bool bAsc)
{
	if (bIsNumeric)
	{
		if (bAsc)
			pWnd->SortItems(CompareAscI, 0L);
		else
			pWnd->SortItems(CompareDesI, 0L);
	}
	else
	{
		if (bAsc)
			pWnd->SortItems(CompareAsc, 0L);
		else
			pWnd->SortItems(CompareDes, 0L);
	}
}

int CALLBACK CSortClass::CompareAsc(LPARAM lParam1, LPARAM lParam2, LPARAM )
{
	CSortItem * i1 = (CSortItem *) lParam1;
	CSortItem * i2 = (CSortItem *) lParam2;
	ASSERT(i1 && i2);
	int res = i1->txt.CompareNoCase(i2->txt);
	if( res == 0 )
	{
		if( i1->indent == i2->indent )
			res = 0;
		else if( i1->indent > i2->indent )
			res = 1;
		else
			res = -1;
	}
	return res;
}

int CALLBACK CSortClass::CompareDes(LPARAM lParam1, LPARAM lParam2, LPARAM l3 )
{
	int res = CompareAsc( lParam1, lParam2, l3 );
	return -res;
}

int CALLBACK CSortClass::CompareAscI(LPARAM lParam1, LPARAM lParam2, LPARAM )
{
	CSortItemInt * i1 = (CSortItemInt *) lParam1;
	CSortItemInt * i2 = (CSortItemInt *) lParam2;
	ASSERT(i1 && i2);

	int res = 0;
	if (i1->iInt == i2->iInt)
	{
		if( i1->indent == i2->indent )
			res = 0;
		else if( i1->indent > i2->indent )
			res = 1;
		else
			res = -1;
	}
	else
	{
		res = i1->iInt > i2->iInt ? 1 : -1;
	}
	return res;
}

int CALLBACK CSortClass::CompareDesI(LPARAM lParam1, LPARAM lParam2, LPARAM l3 )
{
	int res = CompareAscI( lParam1, lParam2, l3 );
	return -res;
}

CSortClass::CSortItem::CSortItem( DWORD _dw, int _indent, const CString & _txt)
{
	dw = _dw;
	indent = _indent;
	txt = _txt;
}

CSortClass::CSortItem::~CSortItem()
{
}

CSortClass::CSortItemInt::CSortItemInt( DWORD _dw, int _indent, const CString & _txt)
{
	iInt = _ttoi(_txt);
	indent = _indent;
	dw = _dw;
}

void CSystemInfoListCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CListCtrl::OnSize(nType, cx, cy);

//	this->InvalidateRect( NULL );
}

