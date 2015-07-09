#include "stdafx.h"
#include "ApiHooks.h"
#include "TaskManagerExDll.h"

HOOKFUNCDESC		pUser32APIHooks[USER32APIHOOKCOUNT];
PROC				pUser32APIOrigins[USER32APIHOOKCOUNT];

BOOL WINAPI SetMenuHooked(
	HWND hWnd,  // handle to window
	HMENU hMenu // handle to menu
)
{
	static bool bFirst = true;
	if (bFirst && g_bMoreLogging)
	{
		bFirst = false;
		TRACE(_T("TaskManagerEx.dll> TaskManagerExDllApp::SetMenuHooked: wnd = 0x%IX;\n"), hWnd);
	}

	if ( hWnd == theApp.hwndTaskManager )
	{
//		TRACE( _T("SetMenuHooked\n") );
		theApp.UpdateMainMenu( hMenu );
	}

	if ( pUser32APIOrigins[HOOKSETMENU] != NULL )
		return ((FnSetMenu)pUser32APIOrigins[HOOKSETMENU])( hWnd, hMenu );
	else
		return FALSE;
}

BOOL HookImportedFunctions()
{
	UINT uiHooked = 0;

	::ZeroMemory( pUser32APIHooks, sizeof(pUser32APIHooks) );
	::ZeroMemory( pUser32APIOrigins, sizeof(pUser32APIOrigins) );

	pUser32APIHooks[HOOKSETMENU].pProc = (PROC)SetMenuHooked;
	pUser32APIHooks[HOOKSETMENU].szFunc = "SetMenu";

	HookImportFunctionsByName( 
						(HINSTANCE)GetWindowLongPtr( theApp.hwndTaskManager, GWLP_HINSTANCE ),
						"user32.dll",
						USER32APIHOOKCOUNT, 
						pUser32APIHooks, 
						pUser32APIOrigins, 
						&uiHooked );

	return TRUE;
}
