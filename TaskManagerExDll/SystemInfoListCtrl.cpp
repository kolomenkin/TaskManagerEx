// SystemInfoListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "taskmanagerexdll.h"
#include "SystemInfoListCtrl.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SystemInfoListCtrl

SystemInfoListCtrl::SystemInfoListCtrl()
{
   nSortedCol = -1; 
   bSortAscending = TRUE;
}

SystemInfoListCtrl::~SystemInfoListCtrl()
{
}


BEGIN_MESSAGE_MAP(SystemInfoListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(SystemInfoListCtrl)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_NOTIFY(HDN_ITEMCLICKA, 0, OnHeaderClicked) 
   ON_NOTIFY(HDN_ITEMCLICKW, 0, OnHeaderClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SystemInfoListCtrl message handlers

int SystemInfoListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

  	SetExtendedStyle( GetExtendedStyle() |
  	   LVS_EX_FULLROWSELECT |
  	   LVS_EX_HEADERDRAGDROP );

	return 0;
}

void SystemInfoListCtrl::PreSubclassWindow() 
{
	SetExtendedStyle( GetExtendedStyle() |
  	   LVS_EX_FULLROWSELECT |
  	   LVS_EX_HEADERDRAGDROP );
	
	CListCtrl::PreSubclassWindow();
}

void SystemInfoListCtrl::OnHeaderClicked(NMHDR* pNMHDR, LRESULT* pResult) 
{
   HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;

   if( phdn->iButton == 0 )
   {
      // User clicked on header using left mouse button
      if( phdn->iItem == nSortedCol )
         bSortAscending = !bSortAscending;
      else
         bSortAscending = TRUE;

      nSortedCol = phdn->iItem;

      HDITEM hi;
      ::ZeroMemory( &hi, sizeof(hi) );
      hi.mask = HDI_FORMAT;

      GetHeaderCtrl()->GetItem( phdn->iItem, &hi );

     	CSortClass csc(this, nSortedCol, hi.fmt & LVCFMT_RIGHT );
   	csc.Sort( bSortAscending == TRUE );
   }

	*pResult = 0;
}

int SystemInfoListCtrl::FindColumn( LPCTSTR lpszColumn )
{
   int nCol = -1;
   HDITEM headerInfo;
   TCHAR  lpBuffer[256];

   headerInfo.mask = HDI_TEXT;
   headerInfo.pszText = lpBuffer;
   headerInfo.cchTextMax = 256;

   for ( int i = 0; i < GetHeaderCtrl()->GetItemCount(); i++ )
   {
      GetHeaderCtrl()->GetItem( i, &headerInfo );

      if ( _tcscmp( lpszColumn, headerInfo.pszText ) == 0 )
         return i;
   }

   return nCol;   
}

BOOL SystemInfoListCtrl::GetSubItemText( int nPos, LPCTSTR lpszColumn, CString& subItemText )
{
   int nCol = FindColumn( lpszColumn );

   if ( nCol == -1 )
      return FALSE;

   subItemText = GetItemText( nPos, nCol );

   return TRUE;
}

BOOL SystemInfoListCtrl::GetSelectedSubItemText( int nCol, CString& subItemText )
{
   POSITION pos = GetFirstSelectedItemPosition();

   if ( pos == NULL )
      return FALSE;

   subItemText = GetItemText( GetNextSelectedItem( pos ), nCol );

   return pos == NULL;
}

BOOL SystemInfoListCtrl::GetSelectedSubItemText( LPCTSTR lpszColumn, CString& subItemText )
{
   int nCol = FindColumn( lpszColumn );

   if ( nCol == -1 )
      return FALSE;

   return GetSelectedSubItemText( nCol, subItemText );
}

void SystemInfoListCtrl::OnContextMenu(CWnd* pWnd, CPoint) 
{
   GetParent()->SendMessage( WM_CONTEXTMENU, (WPARAM)pWnd->GetSafeHwnd() );
}

void SystemInfoListCtrl::OnPaint() 
{
    Default();
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
        pHC = GetHeaderCtrl();
        if (pHC != NULL)
        {
            CRect rcH;
            pHC->GetItemRect(0, &rcH);
            rc.top += rcH.bottom;
        }
        rc.top += 10;

        CString strText(_T("There are no items to show.")); // The message you want!
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
	DWORD dw;
	CString txt;
	if (bIsNumeric)
	{
		for (int t = 0; t < max; t++)
		{
			dw = pWnd->GetItemData(t);
			txt = pWnd->GetItemText(t, iCol);
			pWnd->SetItemData(t, (DWORD) new CSortItemInt(dw, txt));
		}
	}
	else
	{
		for (int t = 0; t < max; t++)
		{
			dw = pWnd->GetItemData(t);
			txt = pWnd->GetItemText(t, iCol);
			pWnd->SetItemData(t, (DWORD) new CSortItem(dw, txt));
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
	return i1->txt.CompareNoCase(i2->txt);
}

int CALLBACK CSortClass::CompareDes(LPARAM lParam1, LPARAM lParam2, LPARAM )
{
	CSortItem * i1 = (CSortItem *) lParam1;
	CSortItem * i2 = (CSortItem *) lParam2;
	ASSERT(i1 && i2);
	return i2->txt.CompareNoCase(i1->txt);
}

int CALLBACK CSortClass::CompareAscI(LPARAM lParam1, LPARAM lParam2, LPARAM )
{
	CSortItemInt * i1 = (CSortItemInt *) lParam1;
	CSortItemInt * i2 = (CSortItemInt *) lParam2;
	ASSERT(i1 && i2);
	if (i1->iInt == i2->iInt) return 0;
	return i1->iInt > i2->iInt ? 1 : -1;
}

int CALLBACK CSortClass::CompareDesI(LPARAM lParam1, LPARAM lParam2, LPARAM )
{
	CSortItemInt * i1 = (CSortItemInt *) lParam1;
	CSortItemInt * i2 = (CSortItemInt *) lParam2;
	ASSERT(i1 && i2);
	if (i1->iInt == i2->iInt) return 0;
	return i1->iInt < i2->iInt ? 1 : -1;
}

CSortClass::CSortItem::CSortItem(const DWORD _dw, const CString & _txt)
{
	dw = _dw;
	txt = _txt;
}

CSortClass::CSortItem::~CSortItem()
{
}

CSortClass::CSortItemInt::CSortItemInt(const DWORD _dw, const CString & _txt)
{
	iInt = atoi(_txt);
	dw = _dw;
}
