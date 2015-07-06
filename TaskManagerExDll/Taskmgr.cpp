#include "StdAfx.h"
#include "Taskmgr.h"

HWND FindTaskManagerWindow()
{
	return ::FindWindow( TASKMGR_DEFAULT_WINDOW_CLASS, GetTaskManagerCaption() );
}

LPCTSTR GetEnglishTaskManagerCaption()
{
	return TASKMGR_DEFAULT_WINDOW_CAPTION;
}

// Thanks Dominique Faure for the idea
// dominique.faure@msg-software.com
LPCTSTR GetTaskManagerCaption()
{
	static LPCTSTR lpszCaption = NULL;
	static TCHAR szCaption[0x100];

	if ( lpszCaption != NULL )
	{
		return lpszCaption;
	}

	// Note! LoadLibraryEx( LOAD_LIBRARY_AS_DATAFILE ) is more correct as LoadLibrary()!
	HINSTANCE hTaskmgrExe = LoadLibraryEx( TASKMGR_LIBRARY, NULL, LOAD_LIBRARY_AS_DATAFILE );

	if ( hTaskmgrExe != NULL )
	{
		int iRet = LoadString( hTaskmgrExe, TASKMGR_IDS_WINDOW_CAPTION, szCaption, 0x100 );
		FreeLibrary( hTaskmgrExe );
		if( iRet > 0 )
		{
			//TRACE( _T("TaskManagerEx: Caption of taskmgr.exe found OK!\n") );
			lpszCaption = szCaption;
		}
	}

	if ( lpszCaption == NULL )
	{
		TRACE( _T("TaskManagerEx: ERROR: Can't find caption of taskmgr.exe!\n") );
		lstrcpy( szCaption, GetEnglishTaskManagerCaption() );
		lpszCaption = szCaption;
	}

	return lpszCaption;
}
