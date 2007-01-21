//////////////////////////////////////////////////////////////
// File:		// SplashWnd.cpp
// File time:	// 21.04.2005	11:15
// Version: 	// 1.0
// Author:		// Sergey Kolomenkin <register@tut.by>
// Copyright:	// (c) Sergey Kolomenkin, Minsk, 2005
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#include "stdafx.h"
//#include <windows.h>
#include "SplashWnd.h"
#include "MakeWindowTransparent.h"
#include "Resource.h"

#include "../TaskManagerExDll/Localization.h"
#include "../TaskManagerExDll/TaskManagerEx.h"
#include "../TaskManagerExDll/product.h"

#include <atlbase.h>
#include <atlconv.h>

//////////////////////////////////////////////////////////////

#pragma  comment (lib, "gdi32.lib")

//////////////////////////////////////////////////////////////

//#include <stdlib.h>

DWORD	g_dwSplashScreenTimeToLive	= 0;
BOOL	g_bStopSplashOnTimer		= TRUE;

const COLORREF crText			= RGB(59,126,236); // RGB(150,210,125);
const COLORREF crTextBorder		= RGB(0,0,0);
const COLORREF crDarkBorder		= RGB( 9, 63, 155 );
const COLORREF crTransparent	= RGB(255,255,255);
const COLORREF crBackground		= RGB(185,223,221); // crTransparent;

//#define EXIT_AFTER_SPLASH
//#define EXIT_ON_MOUSE_CLICK
//#define EXIT_ON_KBD_HIT
//#define TOGGLE_SPLASH	// dubug purposes

const int iSplashWidth			= 550;
const int iSplashHeight			= 150;

const int iSplashInternalWidth	= 500;
const int iSplashInternalHeight	= 110;

const int iTextHeightName		= 40;	// percent of window height
const int iTextHeightVersion	= 30;	// percent of window height
const int iTextHeightCopy1		= 15;	// percent of window height
const int iTextHeightCopy2		= 15;	// percent of window height

const int iCharWidthVersion		= 80;	// percent of Name char width
const int iCharWidthCopy1		= 32;	// percent of Name char width
const int iCharWidthCopy2		= 32;	// percent of Name char width

const BYTE transparentFactor	= 200;

HFONT MakeFont( int CharWidth, int CharHeight )
{
	HFONT hf = CreateFont( CharHeight, CharWidth, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
		NULL);
	return hf;
}

void MyDrawText( HDC hdc, int y, LPCTSTR szText, int CharWidth, int CharHeight, int WindowWidth,
			  COLORREF ColorText, COLORREF ColorBorder )
{
	SIZE sz;

	HFONT	hf		= MakeFont( CharWidth, CharHeight );
	HBRUSH	hbr		= ::CreateSolidBrush( ColorText );
	HPEN	hpen	= ::CreatePen( PS_SOLID, 1, ColorBorder );

	HFONT	hf1		= (HFONT)	::SelectObject(hdc, hf);
	HBRUSH	hbr1	= (HBRUSH )	::SelectObject(hdc, hbr);
	HPEN	hpen1	= (HPEN)	::SelectObject(hdc, hpen);

	int len = lstrlen(szText);
	GetTextExtentPoint( hdc, szText, len, &sz );

	int TextWidth = sz.cx;

	::SetBkMode(hdc, TRANSPARENT);

	::BeginPath(hdc);
	::TextOut(hdc, (WindowWidth - TextWidth)/2, y, szText, len );
	::EndPath(hdc);

	::StrokeAndFillPath(hdc);

	::SelectObject(hdc, hf1);
	::SelectObject(hdc, hbr1);
	::SelectObject(hdc, hpen1);

	::DeleteObject(hf);
	::DeleteObject(hbr);
	::DeleteObject(hpen);
}


void OnSplashWndPaint( HWND hWnd )
{
	PAINTSTRUCT ps;
	HDC		hdc = NULL;
	RECT	rt;

//#ifdef _UNICODE
//#define MY_T2(x)	L#x
//#else
//#define MY_T2(x)	x
//#endif
//#define MY_T(x)		MY_T2(x)

	LPCTSTR szProduct = TMEX_NAME;
	//LPCTSTR szVersion = MY_T( VERSIONPRODUCTVERSIONSTR );
	//LPCTSTR szCopy1 = MY_T( COPYRIGHT1 );
	//LPCTSTR szCopy2 = MY_T( COPYRIGHT2 );

	USES_CONVERSION;
	LPCTSTR szVersion = A2CT( VERSIONPRODUCTVERSIONSTR );
	LPCTSTR szCopy1 = A2CT( COPYRIGHT1 );
	LPCTSTR szCopy2 = A2CT( COPYRIGHT2 );

	hdc = BeginPaint(hWnd, &ps);

	::GetClientRect(hWnd, &rt);
	int width = rt.right - rt.left;
//	int height = rt.bottom - rt.top;

	int charWidth = width/lstrlen(szProduct);
	int MainTextHeight = iSplashInternalHeight*iTextHeightName/100;
	int y = (iSplashHeight - iSplashInternalHeight)/2;
	int TextWidth = 0;

	do
	{
		charWidth--;
		HFONT hf = MakeFont( charWidth, MainTextHeight );
		HFONT hf1 = (HFONT)::SelectObject(hdc, hf);

		SIZE sz;
		GetTextExtentPoint( hdc, szProduct, lstrlen(szProduct), &sz );
		TextWidth = sz.cx;

		::SelectObject(hdc, hf1);
		::DeleteObject( hf );

	} while( TextWidth > iSplashInternalWidth && charWidth > 1 );


	HPEN hPen = CreatePen( PS_SOLID, 1, crDarkBorder ); //GetSysColor(COLOR_WINDOWFRAME) );
	HPEN hPen1 = (HPEN) ::SelectObject(hdc, hPen);
	HBRUSH hBrush = CreateSolidBrush( crBackground );

	::Rectangle( hdc, rt.left, rt.top, rt.right, rt.bottom );

	rt.left++, rt.top++, rt.right--, rt.bottom--; // Inflate rectangle by 1 pixel

	::FillRect( hdc, &rt, hBrush );

	::DeleteObject( hBrush );
	::SelectObject(hdc, hPen1);
	::DeleteObject(hPen);

	int h;

	h = MainTextHeight;
	MyDrawText( hdc, y, szProduct, charWidth, h, width, crText, crTextBorder );
	y += h;

	h = iSplashInternalHeight*iTextHeightVersion/100;
	MyDrawText( hdc, y, szVersion, charWidth*iCharWidthVersion/100, h, width, crText, crTextBorder );
	y += h;

	h = iSplashInternalHeight*iTextHeightCopy1/100;
	MyDrawText( hdc, y, szCopy1, charWidth*iCharWidthCopy1/100, h, width, crText, crDarkBorder );
	y += h;

	h = iSplashInternalHeight*iTextHeightCopy2/100;
	MyDrawText( hdc, y, szCopy2, charWidth*iCharWidthCopy2/100, h, width, crText, crDarkBorder );
	y += h;

	EndPaint(hWnd, &ps);
}

// Following function was cut from MFC sources and adapted.

void CenterWindow( HWND m_hWnd )
{
	ASSERT(::IsWindow(m_hWnd));

	// determine owner window to center against
	DWORD dwStyle = GetWindowLong( m_hWnd, GWL_STYLE );
	HWND hWndCenter = NULL;

	{
		if (dwStyle & WS_CHILD)
			hWndCenter = ::GetParent(m_hWnd);
		else
			hWndCenter = ::GetWindow(m_hWnd, GW_OWNER);
	}

	// get coordinates of the window relative to its parent
	RECT rcDlg;
	::GetWindowRect( m_hWnd, &rcDlg );
	RECT rcArea;
	RECT rcCenter;
	HWND hWndParent;
	if (!(dwStyle & WS_CHILD))
	{
		// don't center against invisible or minimized windows
		if (hWndCenter != NULL)
		{
			DWORD dwStyle = ::GetWindowLong(hWndCenter, GWL_STYLE);
			if (!(dwStyle & WS_VISIBLE) || (dwStyle & WS_MINIMIZE))
				hWndCenter = NULL;
		}
/*
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);

		// center within appropriate monitor coordinates
		if (hWndCenter == NULL)
		{
			HWND hwDefault = m_hWnd;

			GetMonitorInfo(
				MonitorFromWindow(hwDefault, MONITOR_DEFAULTTOPRIMARY), &mi);
			rcCenter = mi.rcWork;
			rcArea = mi.rcWork;
		}
		else
		{
			::GetWindowRect(hWndCenter, &rcCenter);
			GetMonitorInfo(
				MonitorFromWindow(hWndCenter, MONITOR_DEFAULTTONEAREST), &mi);
			rcArea = mi.rcWork;
		}
*/
		int		tx = 0;
		int		ty = 0;
		HWND hDesktopWnd = ::GetDesktopWindow();
		{
			HDC hDc = ::GetDC(hDesktopWnd);
			tx = ::GetDeviceCaps(hDc, HORZRES);
			ty = ::GetDeviceCaps(hDc, VERTRES);
			::ReleaseDC(hDesktopWnd, hDc);
		}

		rcArea.left = 0;
		rcArea.top = 0;
		rcArea.right = tx;
		rcArea.bottom = ty;
		rcCenter = rcArea;
	}
	else
	{
		// center within parent client coordinates
		hWndParent = ::GetParent(m_hWnd);
		ASSERT(::IsWindow(hWndParent));

		::GetClientRect(hWndParent, &rcArea);
		ASSERT(::IsWindow(hWndCenter));
		::GetClientRect(hWndCenter, &rcCenter);
		::MapWindowPoints(hWndCenter, hWndParent, (POINT*)&rcCenter, 2);
	}

	int rcDlg_Width = rcDlg.right - rcDlg.left;
	int rcDlg_Height = rcDlg.bottom - rcDlg.top;

	// find dialog's upper left based on rcCenter
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - rcDlg_Width / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - rcDlg_Height / 2;

	// if the dialog is outside the screen, move it inside
	if (xLeft < rcArea.left)
		xLeft = rcArea.left;
	else if (xLeft + rcDlg_Width > rcArea.right)
		xLeft = rcArea.right - rcDlg_Width;

	if (yTop < rcArea.top)
		yTop = rcArea.top;
	else if (yTop + rcDlg_Height > rcArea.bottom)
		yTop = rcArea.bottom - rcDlg_Height;

	// map screen coordinates to child coordinates
	::SetWindowPos( m_hWnd, NULL, xLeft, yTop, -1, -1,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL	g_bDragSplash = FALSE;
POINT	g_ptMouseDragBegin;
RECT	g_rWindowDragBegin;

LRESULT CALLBACK SplashWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int iRes;
	switch (message) 
	{
		case WM_CREATE:
			TRACE( _T("SplashWndProc> WM_CREATE\n") );
			iRes = SetTimer( hWnd, 1, g_dwSplashScreenTimeToLive, NULL );
//			InvalidateRect( hWnd, NULL, FALSE );
//			OnSplashWndPaint(hWnd);
			CenterWindow( hWnd );
			MakeWindowTransparent( hWnd, transparentFactor, crTransparent );
			SetCursor( LoadCursor( NULL, MAKEINTRESOURCE(IDC_ARROW) ) );
			break;

		case WM_PAINT:
//			TRACE( _T("SplashWndProc> WM_PAINT\n") );
			OnSplashWndPaint(hWnd);
			break;

		case WM_LBUTTONUP:
			if( g_bDragSplash )
			{
				g_bDragSplash = FALSE;
				ReleaseCapture();
			}
			break;

		case WM_MOUSEMOVE:
			if( g_bDragSplash )
			{
				POINT	p1 = g_ptMouseDragBegin;
				RECT	r = g_rWindowDragBegin;
				POINT	p2;
				GetCursorPos( &p2 );
				int dx = p2.x - p1.x;
				int dy = p2.y - p1.y;

				MoveWindow( hWnd, r.left + dx, r.top + dy, r.right - r.left, r.bottom - r.top, TRUE );
			}
			break;

		case WM_LBUTTONDOWN:
			#ifdef EXIT_ON_MOUSE_CLICK
				::DestroyWindow(hWnd);
			#else
			SetCapture(hWnd);
			GetCursorPos( &g_ptMouseDragBegin );
			GetWindowRect( hWnd, &g_rWindowDragBegin );
			g_bDragSplash = TRUE;	// Set flag to check for key down in mouse move
									// message.
			#endif
			break;
			
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
//			TRACE( _T("SplashWndProc> Mouse click\n") );
			#ifdef EXIT_ON_MOUSE_CLICK
				::DestroyWindow(hWnd);
			#else
				#ifdef TOGGLE_SPLASH
					g_bStopSplashOnTimer = !g_bStopSplashOnTimer;
				#endif
			#endif
			break;


		case WM_KEYDOWN:
//			TRACE( _T("SplashWndProc> WM_KEYDOWN: VK = %X\n"), wParam );
			if( wParam == VK_ESCAPE )
			{
				::DestroyWindow(hWnd);
			}
			else
			{
				#ifdef EXIT_ON_KBD_HIT
					::DestroyWindow(hWnd);
				#else
					#ifdef TOGGLE_SPLASH
						g_bStopSplashOnTimer = !g_bStopSplashOnTimer;
					#endif
				#endif
			}

			//return DefWindowProc(hWnd, message, wParam, lParam);
			break;

		case WM_TIMER:
			TRACE( _T("SplashWndProc> WM_TIMER\n") );
			if( g_bStopSplashOnTimer )
			{
				::DestroyWindow(hWnd);
			}
			break;

		case WM_DESTROY:
			TRACE( _T("SplashWndProc> WM_DESTROY\n") );
			#ifdef EXIT_AFTER_SPLASH
				PostQuitMessage( 0 );
			#endif
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

void RegisterWndClass( LPCTSTR szWindowClass )
{
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(wcex); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)SplashWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;

	RegisterClassEx(&wcex);
}

BOOL CreateSplashScreen( DWORD milliseconds )
{
	TCHAR szWindowClass[] = _T("SpashWindowCLASS");

	// -------------------------------------------------------

	HWND hParentWnd = NULL; // ::GetDesktopWindow();
	int tx = iSplashWidth;
	int ty = iSplashHeight;

	// -------------------------------------------------------------

	g_dwSplashScreenTimeToLive = milliseconds;
	g_bStopSplashOnTimer = TRUE;
	g_bDragSplash = FALSE;

	RegisterWndClass( szWindowClass );

//	HWND hForeWnd = GetForegroundWindow();

	HWND hWndUser = CreateWindowEx(
		WS_EX_TOPMOST, szWindowClass,
		TMEX_NAME, WS_VISIBLE | WS_POPUP,
		0, 0, tx, ty, hParentWnd, NULL, NULL, NULL);

	ShowWindow(hWndUser, SW_SHOW);
//	SetForegroundWindow( hForeWnd ); // It works strange ????

	return TRUE;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
