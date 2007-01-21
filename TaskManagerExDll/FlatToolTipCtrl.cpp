// FlatToolTipCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "FlatToolTipCtrl.h"

// CToolTipCtrlEx

IMPLEMENT_DYNAMIC(CToolTipCtrlEx, CToolTipCtrl)
CToolTipCtrlEx::CToolTipCtrlEx()
{
	m_bkColor		= GetSysColor(COLOR_INFOBK); // RGB(255,255,255);
	m_leftColor		= RGB(255, 210, 83); // RGB(255, 115, 50)
	m_frameColor	= GetSysColor(COLOR_WINDOWFRAME); // RGB(155, 110, 53);
	m_textColor		= GetSysColor(COLOR_INFOTEXT); // RGB(0,0,0)
}

CToolTipCtrlEx::~CToolTipCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CToolTipCtrlEx, CToolTipCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_NCPAINT()
	ON_WM_PAINT()
END_MESSAGE_MAP()


void CToolTipCtrlEx::PreSubclassWindow()
{
	CToolTipCtrl::PreSubclassWindow();
}

BOOL CToolTipCtrlEx::OnEraseBkgnd(CDC* pDC)
{
	return CToolTipCtrl::OnEraseBkgnd(pDC);
}

void CToolTipCtrlEx::OnNcPaint()
{
}


// CToolTipCtrlEx message handlers

void CToolTipCtrlEx::OnPaint()
{
	POINT Pos;
	GetCursorPos(&Pos);

	ICONINFO IconInfo;
	ZeroMemory( &IconInfo, sizeof(IconInfo) );
	HCURSOR hCursor = GetCursor();
	GetIconInfo( (HICON) hCursor, &IconInfo );

	DWORD CursorHeight = 0;
	BITMAP CursorBitmap;
	ZeroMemory( &CursorBitmap, sizeof(CursorBitmap) );
	GetObject( IconInfo.hbmMask,
		sizeof(CursorBitmap), &CursorBitmap );

	if( IconInfo.hbmColor == NULL )
	{
		CursorHeight = CursorBitmap.bmHeight / 2;
	}
	else
	{
		CursorHeight = CursorBitmap.bmHeight;
	}

	CRect PRect(0,0,0,0);
	CWnd* pChild = WindowFromPoint(Pos);
	if( pChild != NULL )
	{
		pChild->GetWindowRect(PRect);
	}

	CPaintDC dc(this); // device context for painting
	CRect	Rect,R1;
	CRect	WRect;

	CFont* pOldFont = dc.SelectObject(GetFont());

	CString	Tip;
	GetWindowText(Tip);

	UINT Width = 0;
	UINT Rows = 0;
	UINT iPos = 0;
	Tip.Remove( _T('\r') );
	Tip.Replace( _T("\t"), _T("    ") );

	int pos = -1;
	do
	{
		int old_pos = pos;
		pos = Tip.Find( _T('\n'), pos+1 );
		CString TStr;
		if( pos == -1 )
		{
			TStr = Tip.Mid( old_pos+1 );
		}
		else
		{
			TStr = Tip.Mid( old_pos+1, pos - old_pos - 1 );
		}
		CSize Sz1 = dc.GetTextExtent(TStr);
		Width = max( Width, (UINT)Sz1.cx );
		Rows += 1;
	} while( pos != -1 );

	GetWindowRect(WRect);
	TEXTMETRIC TM;
	dc.GetTextMetrics(&TM);

	DWORD BorderWidth = 1;
	DWORD LeftWidth = 12;
	DWORD TextHorzIndentation = 4;
	DWORD TextVertIndentation = 1;

	WRect.top = Pos.y
		- IconInfo.yHotspot // HotSpot (Cursor position) may be inside cursor
		+ CursorHeight;
	WRect.bottom = WRect.top + Rows * (TM.tmHeight)
		+ TextVertIndentation*2 + BorderWidth*2;

	UINT T = WRect.Width() - (LeftWidth+Width);

	WRect.left = Pos.x;
	WRect.right = WRect.left + LeftWidth + Width
		+ TextHorzIndentation*2 + BorderWidth*2;

	if(WRect.right > GetSystemMetrics(SM_CXSCREEN)-25)
	{
		WRect.OffsetRect(-(WRect.right-(GetSystemMetrics(SM_CXSCREEN)-25)),0);
	}

	MoveWindow( &WRect );

	GetClientRect( Rect );
	dc.FillSolidRect( Rect, m_bkColor );
	R1 = Rect;
	R1.right = R1.left + LeftWidth;
	dc.FillSolidRect( R1, m_leftColor );
	Rect.left = R1.right;

	dc.SetBkMode( TRANSPARENT );
	Rect.top += TextVertIndentation;
	Rect.bottom -= TextVertIndentation;
	Rect.left += TextHorzIndentation;
	Rect.right -= TextHorzIndentation;

	dc.SetTextColor(m_textColor);

	if( Rows == 1 )
	{
		dc.DrawText( Tip, Rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
	}
	else
	{
		dc.DrawText( Tip, Rect, DT_LEFT | DT_TOP );
	}

	GetClientRect( Rect );
	dc.Draw3dRect( Rect, m_frameColor, m_frameColor );

	dc.SelectObject( pOldFont );
}

