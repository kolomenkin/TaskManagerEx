//////////////////////////////////////////////////////////////
// File:		// Options.h
// File time:	// 16.04.2005	0:53
// Version: 	// 1.0
// Author:		// Sergey Kolomenkin <register@tut.by>
// Copyright:	// (c) Sergey Kolomenkin, Minsk, 2005
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#ifndef _OPTIONS_H_UID0000002B0A6B39CE
#define _OPTIONS_H_UID0000002B0A6B39CE

//////////////////////////////////////////////////////////////////////////

#include "TaskManagerEx.h"

//////////////////////////////////////////////////////////////////////////
// Tip Of The Day options:

#define g_szTipsSection			_T("")
#define g_szTipsIntFilePos		_T("TipsLastPos")
#define g_szTipsIntStartup		_T("TipsShowAtStartUp")
#define g_bStartTipsByDefault	FALSE

//////////////////////////////////////////////////////////////////////////

template<class TYPE, UINT nID, TYPE DefaultValue>
class CSimpleProperty
{
public:

	void operator=( const TYPE& a )
	{
		Save( a );
	}

	operator TYPE()
	{
		
		return Load();
	}

protected:

	TYPE Load()
	{
		TYPE result = DefaultValue;
		LPCTSTR strRegValue = COptions::m_szRegValues[nID];
		HKEY hKey = NULL;
		DWORD dwType = REG_DWORD;
		DWORD dwSize = sizeof(result);
		ASSERT( sizeof(TYPE) == sizeof(DWORD) );

		::RegOpenKeyEx( REG_ROOT, REG_KEY, 0, KEY_READ, &hKey );
		if( hKey != NULL )
		{
			::RegQueryValueEx( hKey, strRegValue, NULL, &dwType, 
				   (LPBYTE)&result, &dwSize );
			::RegCloseKey( hKey );
		}

		return result;
	}

	void Save( const TYPE& a )
	{
		LPCTSTR strRegValue = COptions::m_szRegValues[nID];
		HKEY hKey = NULL;
		ASSERT( sizeof(TYPE) == sizeof(DWORD) );

		::RegCreateKeyEx( REG_ROOT, REG_KEY, 0, NULL,
			REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL );
		if( hKey != NULL )
		{
			::RegSetValueEx( hKey, strRegValue, NULL, REG_DWORD, 
					   (const BYTE*)&a, sizeof(a) );
			::RegCloseKey( hKey );
		}
	}
};

//////////////////////////////////////////////////////////////////////////

#define AUTORUN_ROOT	HKEY_LOCAL_MACHINE
#define AUTORUN_KEY		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run")
#define AUTORUN_VALUE	_T("TaskManagerEx")

#ifndef TASKMANEX_EXE

CString GetTaskManagerExe();

#endif

//////////////////////////////////////////////////////////////////////////

class COptions
{
public:

	BOOL Uninstall()
	{
		// Remove TaskManagerEx from startup:
		HKEY hKey = NULL;
		::RegOpenKeyEx( AUTORUN_ROOT, AUTORUN_KEY, 0, KEY_SET_VALUE, &hKey );
		if( hKey != NULL )
		{
			::RegDeleteValue( hKey, AUTORUN_VALUE );
			::RegCloseKey( hKey );
		}
		return TRUE;
	}

#ifndef TASKMANEX_EXE

	BOOL IsAutorun()
	{
		BOOL result = FALSE;

		TCHAR szBuf[MAX_PATH] = _T("");
		HKEY hKey = NULL;
		DWORD dwType = REG_SZ;
		DWORD dwSize = sizeof(szBuf);

		::RegOpenKeyEx( AUTORUN_ROOT, AUTORUN_KEY, 0, KEY_QUERY_VALUE, &hKey );
		if( hKey != NULL )
		{
			::RegQueryValueEx( hKey, AUTORUN_VALUE, NULL, &dwType, 
				   (LPBYTE)szBuf, &dwSize );
			if( lstrlen(szBuf) > 0 )
			{
				result = TRUE;
			}
			::RegCloseKey( hKey );
		}

		return result;
	}

	void SetAutorun( BOOL bEnable )
	{
		HKEY hKey = NULL;
		::RegOpenKeyEx( AUTORUN_ROOT, AUTORUN_KEY, 0, KEY_SET_VALUE, &hKey );
		if( hKey != NULL )
		{
			if( bEnable )
			{
				CString szExe = GetTaskManagerExe();
				::RegSetValueEx( hKey, AUTORUN_VALUE, 0, REG_SZ, 
					   (LPBYTE)(LPCTSTR)szExe, (szExe.GetLength()+1)*sizeof(TCHAR) );
			}
			else
			{
				::RegDeleteValue( hKey, AUTORUN_VALUE );
			}

			::RegCloseKey( hKey );
		}

		bEnable = bEnable;
	}

#endif

	UINT GetAlertLimit()
	{
		return m_iProcessUsageAlertLimit;
	}
	void SetAlertLimit( UINT limit )
	{
		m_iProcessUsageAlertLimit = limit;
	}

	COLORREF GetAlertColor()
	{
		return m_clrCPUUsageAlert;
	}
	void SetAlertColor( COLORREF color )
	{
		m_clrCPUUsageAlert = color;
	}

	COLORREF GetProcessColor()
	{
		return m_clrDefaultProcess;
	}
	void SetProcessColor( COLORREF color )
	{
		m_clrDefaultProcess = color;
	}

	COLORREF GetServiceColor()
	{
		return m_clrServiceProcess;
	}
	void SetServiceColor( COLORREF color )
	{
		m_clrServiceProcess = color;
	}

	BOOL GetShowProcessIcons()
	{
		return m_bShowProcessesIcons;
	}
	void SetShowProcessIcons( BOOL bShow )
	{
		m_bShowProcessesIcons = bShow;
	}

	BOOL GetHideServices()
	{
		return m_bHideServiceProcesses;
	}
	void SetHideServices( BOOL bHide )
	{
		m_bHideServiceProcesses = bHide;
	}

	BOOL GetShowSplash()
	{
		return m_bShowSplash;
	}
	void SetShowSplash( BOOL bShow )
	{
		m_bShowSplash = bShow;
	}

	BOOL GetShowTips()
	{
		return m_bShowTips;
	}
	void SetShowTips( BOOL bShow )
	{
		m_bShowTips = bShow;
	}

public:
	COptions (void)
	{
		m_szRegValues[ALERT_LIMIT]			= _T("ProcessUsageAlertLimit");
		m_szRegValues[ALERT_COLOR]			= _T("CPUUsageAlertColor");
		m_szRegValues[PROCESS_COLOR]		= _T("ProcessColor");
		m_szRegValues[SERVICE_COLOR]		= _T("ServiceProcessHideColor");
		m_szRegValues[SHOW_PROCESS_ICONS]	= _T("ShowProcessesIcons");
		m_szRegValues[HIDE_SERVICES]		= _T("HideServiceProcesses");
		m_szRegValues[SHOW_SPLASH]			= _T("DisplaySplashScreen");
		m_szRegValues[SHOW_TIPS]			= g_szTipsIntStartup; // g_szTipsSection must be equal to "" (empty)
	}

	enum VALUES
	{
		ALERT_LIMIT,
		ALERT_COLOR,
		PROCESS_COLOR,
		SERVICE_COLOR,
		SHOW_PROCESS_ICONS,
		HIDE_SERVICES,
		SHOW_SPLASH,
		SHOW_TIPS,
		//////////////////////////
		VALUE_COUNT
	};

	static LPCTSTR	m_szRegValues[VALUE_COUNT];

protected:

	CSimpleProperty<UINT,		ALERT_LIMIT,		0>						m_iProcessUsageAlertLimit;
	CSimpleProperty<COLORREF,	ALERT_COLOR,		RGB( 255, 0, 0 )>		m_clrCPUUsageAlert;
	CSimpleProperty<COLORREF,	PROCESS_COLOR,		RGB( 0, 0, 0 )>			m_clrDefaultProcess;
	CSimpleProperty<COLORREF,	SERVICE_COLOR,		RGB( 192, 192, 192 )>	m_clrServiceProcess;
	CSimpleProperty<BOOL,		SHOW_PROCESS_ICONS,	TRUE>					m_bShowProcessesIcons;
	CSimpleProperty<BOOL,		HIDE_SERVICES,		TRUE>					m_bHideServiceProcesses;
	CSimpleProperty<BOOL,		SHOW_SPLASH,		TRUE>					m_bShowSplash;
	CSimpleProperty<BOOL,		SHOW_TIPS,			g_bStartTipsByDefault>	m_bShowTips;

private:

};

//////////////////////////////////////////////////////////////////////////

#endif //ifndef _OPTIONS_H_UID0000002B0A6B39CE
