// TaskManagerEx.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TaskManagerEx.h"
#include "LoadDll.h"

DWORD    dwWindowsNTMajorVersion = 0;
HWND		hwndTaskManager = NULL;
BOOL     bStopBaby = FALSE;

BOOL IsAnotherTaskManagerEx()
{
   HANDLE hMutex = CreateMutex( NULL, FALSE, _T("TaskManagerExIsAlreadyRunning") );	
   
   DWORD dwer = GetLastError();	

   return (hMutex==NULL || dwer==ERROR_ALREADY_EXISTS);
}

LPCTSTR GetEnglishTaskManagerCaption()
{
   LPCTSTR rc = NULL;

   switch ( dwWindowsNTMajorVersion )
   {
   case 4:
      rc = _T("Windows NT Task Manager");
      break;

   case 5:
   default:
      rc = _T("Windows Task Manager");
      break;
   }

   return rc;
}

// Thanks Dominique Faure for the idea
// dominique.faure@msg-software.com
LPCTSTR GetTaskManagerCaption()
{
   static LPCTSTR lpszCaption = NULL;
   static TCHAR szCaption[0x100];

   _tcscpy( szCaption, GetEnglishTaskManagerCaption() );

   if ( lpszCaption == NULL )
   {
      HINSTANCE hTaskmgrExe = LoadLibrary( _T("taskmgr.exe") );
      
      if ( hTaskmgrExe != NULL )
      {
         LoadString( hTaskmgrExe, 10003, szCaption, 0x100 );
         FreeLibrary( hTaskmgrExe );
      }

      lpszCaption = szCaption;
   }

   return lpszCaption;
}

BOOL FindTaskManagerWindow()
{
	hwndTaskManager = ::FindWindow( NULL, GetTaskManagerCaption() );

	return hwndTaskManager != NULL;
}

DWORD GetTaskManagerProcessID()
{
	DWORD pID = 0;

	GetWindowThreadProcessId( hwndTaskManager, &pID );

	return pID;
}

int CALLBACK WinMain( HINSTANCE, HINSTANCE, LPTSTR, int )
{
   if ( IsAnotherTaskManagerEx() )
      return 0;

   TCHAR szDllPath[_MAX_PATH] = _T("");

   dwWindowsNTMajorVersion = IsWindowsNT();
   
	if ( dwWindowsNTMajorVersion < 4 )
	{
	   MessageBox( 0, _T("You need Windows NT to run this application"), _T("Error"), MB_OK );
		return -1;
   }

   if ( GetModuleFileName( NULL, szDllPath, _MAX_PATH ) == 0 )
   {
      MessageBox( 0, _T("GetModuleFileName() failed"), _T("Error"), MB_OK );
      return -2;
   }

   _tcscpy( szDllPath + _tcslen(szDllPath) - 4, _T(".dll") );

	//Let's work
	
	while( !bStopBaby )
	{
		if ( FindTaskManagerWindow() )
		   LoadDllForRemoteThread( 
					GetTaskManagerProcessID(), 
					TRUE, 
					FALSE, 
					szDllPath,
					"Initialize",
					NULL,
					NULL,
					NULL,
					NULL );
		
   	Sleep( 1000 );
	}

	return 0;
}

