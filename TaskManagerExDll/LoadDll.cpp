// Modified by Zoltan Csizmadia, zoltan_csizmadia@yahoo.com
// For companies(Austin,TX): If you would like to get my resume, send an email.
//
//******************************************************************************
//
// Original version made by Felix Kasza, http://www.mvps.org/win32/
//
// demonstrates getting the command line of another process
// requires PROCESS_CREATE_THREAD, PROCESS_VM_OPERATION,
// PROCESS_VM_WRITE, PROCESS_VM_READ to the target, or the
// SeDebugPrivilege

// *** You *must* remove "/GZ" from the debug build settings ***
// (If you use my (felixk's) project file, this has already happened)

#include "stdafx.h"
//#include "TaskManagerExDll.h"
#include "LoadDll.h"

const DWORD MAXINJECTSIZE = 4096;

DWORD IsWindowsNT()
{
   OSVERSIONINFOEX osvi;
   BOOL bOsVersionInfoEx;
   
   // Try calling GetVersionEx using the OSVERSIONINFOEX structure,
   // which is supported on Windows 2000.
   //
   // If that fails, try using the OSVERSIONINFO structure.

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi);

   if( bOsVersionInfoEx == 0 )
   {
      // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.

      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
         return FALSE;
   }

   return ( osvi.dwPlatformId == VER_PLATFORM_WIN32_NT ? osvi.dwMajorVersion : 0 );
}

PVOID GetFuncAddress(PVOID addr)
{
#ifdef _DEBUG
 //check if instruction is relative jump (E9)
 if (0xE9 != *((UCHAR*)addr))
  return addr;

 // calculate base of relative jump
 ULONG base = (ULONG)((UCHAR*)addr + 5);

 // calculate offset 
 ULONG *offset = (ULONG*)((UCHAR*)addr + 1);

 return (PVOID)(base + *offset);
#else
 // in release, don't have to mess with jumps
 return addr;
#endif
}


DWORD LoadDllForRemoteThread( DWORD processID, BOOL bLoad, BOOL bFree, LPCTSTR lpModuleName, char* lpFunctionName, 
   DWORD* pErrorLoad, DWORD* pErrorFunction, DWORD* pReturnCodeForFunction, DWORD* pErrorFree )
{
   DWORD rc = 0;

	//Enable debug privilege
	EnableDebugPriv();

	// open the process
	HANDLE hProcess = OpenProcess( PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |
			PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, processID );
	
	if ( hProcess != NULL )
	{
		// let's work
		rc = ExecuteRemoteThread( hProcess, bLoad, bFree, lpModuleName, lpFunctionName,
		         pErrorLoad, pErrorFunction, pReturnCodeForFunction, pErrorFree );

		CloseHandle( hProcess );
	}
		
	return rc;
}

void EnableDebugPriv( void )
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	// enable the SeDebugPrivilege
	if ( ! OpenProcessToken( GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) )
		return;

	if ( ! LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &sedebugnameValue ) )
	{
		CloseHandle( hToken );
		return;
	}

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges( hToken, FALSE, &tkp, sizeof tkp, NULL, NULL );
		
	CloseHandle( hToken );
}

DWORD ExecuteRemoteThread( HANDLE hProcess, BOOL bLoad, BOOL bFree, LPCTSTR lpDllPath, char* lpFunctionName, 
   DWORD* pErrorLoad, DWORD* pErrorFunction, DWORD* pReturnCodeForFunction, DWORD* pErrorFree )
{
	HANDLE ht = 0;
	void *p = 0;
	RemoteDllThreadBlock *c = 0;
	DWORD rc = (DWORD)-1;
	HMODULE hKernel32 = 0;
	RemoteDllThreadBlock localCopy;

	// clear the parameter block
	::ZeroMemory( &localCopy, sizeof(localCopy) );

	// allocate memory for injected code
	p = VirtualAllocEx( hProcess, 0, MAXINJECTSIZE, MEM_COMMIT, PAGE_EXECUTE_READWRITE );
	if ( p == 0 )
		goto cleanup;

	// allocate memory for parameter block
	c = (RemoteDllThreadBlock*) VirtualAllocEx( hProcess, 0, sizeof(RemoteDllThreadBlock), MEM_COMMIT, PAGE_READWRITE );
	if ( c == 0 )
		goto cleanup;

	// copy function there, we will execute this piece of code
	if ( ! WriteProcessMemory( hProcess, p, GetFuncAddress(RemoteDllThread), MAXINJECTSIZE, 0 ) )
		goto cleanup;
	
	// copy dll path to parameter block
	_tcscpy( localCopy.lpModulePath, lpDllPath );

	if ( lpFunctionName == NULL )
		localCopy.lpFunctionName[0] = 0;
	else
		strcpy( localCopy.lpFunctionName, lpFunctionName );
	
	localCopy.bLoadLibrary = bLoad;
	localCopy.bFreeLibrary = bFree;

	// kernel32.dll
	hKernel32 = GetModuleHandle( _T("kernel32.dll") );

	if ( hKernel32 == NULL )
		goto cleanup;
	
	// get the addresses for the functions, what we will use in the remote thread

#if defined(UNICODE) || defined(_UNICODE)
	localCopy.fnLoadLibrary = (PLoadLibrary)GetProcAddress( hKernel32, "LoadLibraryW" );
#else
   localCopy.fnLoadLibrary = (PLoadLibrary)GetProcAddress( hKernel32, "LoadLibraryA" );
	localCopy.fnGetModuleHandle = (PGetModuleHandle)GetProcAddress( hKernel32, "GetModuleHandleA" );
#endif

	localCopy.fnFreeLibrary = (PFreeLibrary)GetProcAddress( hKernel32, "FreeLibrary" );
	localCopy.fnGetProcAddress = (PGetProcAddress)GetProcAddress( hKernel32, "GetProcAddress" );

	if (localCopy.fnLoadLibrary == NULL ||
		localCopy.fnGetModuleHandle == NULL ||
		localCopy.fnFreeLibrary == NULL ||
		localCopy.fnGetProcAddress == NULL)
		goto cleanup;
		
	// copy the parameterblock to the other process adress space
	if ( ! WriteProcessMemory( hProcess, c, &localCopy, sizeof localCopy, 0 ) )
		goto cleanup;

	// CreateRemoteThread()
	ht = CreateRemoteThread( hProcess, 0, 0, (DWORD (__stdcall *)( void *)) p, c, 0, &rc );
	if ( ht == NULL )
		goto cleanup;
	
	rc = WaitForSingleObject( ht, INFINITE );
	switch ( rc )
	{
	case WAIT_TIMEOUT:
		goto cleanup;

	case WAIT_FAILED:
		goto cleanup;

	case WAIT_OBJECT_0:
		// this might just have worked, pick up the result!
		// rad back the prameter block, it has the error code
		if ( ! ReadProcessMemory( hProcess, c, &localCopy, sizeof localCopy, 0 ) )
			goto cleanup;

      rc = 0;

      if ( pErrorLoad != NULL )
         *pErrorLoad = localCopy.ErrorLoad;

      if ( pErrorFunction != NULL )
         *pErrorFunction = localCopy.ErrorFunction;

      if ( pReturnCodeForFunction != NULL )
         *pReturnCodeForFunction = localCopy.ReturnCodeForFunction;

      if ( pErrorFree != NULL )
         *pErrorFree = localCopy.ErrorFree;

      if ( localCopy.ErrorLoad )
         rc |= 1;

      if ( localCopy.ErrorFunction )
         rc |= 2;

      if ( localCopy.ErrorFree )
         rc |= 4;

		break;
	
	default:
		break;
	}

cleanup:
	CloseHandle( ht );

	// Let's clean
	if ( p != 0 )
		VirtualFreeEx( hProcess, p, 0, MEM_RELEASE );
	if ( c != 0 )
		VirtualFreeEx( hProcess, c, 0, MEM_RELEASE );
	
	return rc;
}

DWORD RemoteGetCurrentDirectory( DWORD pID, LPTSTR lpPath, DWORD size, DWORD* pRetCode )
{
	HANDLE ht = 0;
	void *p = 0;
	RemoteGetCurrentDirectoryThreadBlock *c = 0;
	DWORD rc = (DWORD)-1;
	RemoteGetCurrentDirectoryThreadBlock localCopy;

	HANDLE hProcess = OpenProcess( PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |
			PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pID );

	if ( hProcess == NULL )
	   return 0;

	// clear the parameter block
	::ZeroMemory( &localCopy, sizeof(localCopy) );

	// allocate memory for injected code
	p = VirtualAllocEx( hProcess, 0, MAXINJECTSIZE, MEM_COMMIT, PAGE_EXECUTE_READWRITE );
	if ( p == 0 )
		goto cleanup;

	// allocate memory for parameter block
	c = (RemoteGetCurrentDirectoryThreadBlock*) VirtualAllocEx( 
	         hProcess, 0, sizeof(RemoteGetCurrentDirectoryThreadBlock), MEM_COMMIT, PAGE_READWRITE );

	if ( c == 0 )
		goto cleanup;

	// copy function there, we will execute this piece of code
	if ( ! WriteProcessMemory( hProcess, p, GetFuncAddress(RemoteGetCurrentDirectoryThread), MAXINJECTSIZE, 0 ) )
		goto cleanup;
	
#if defined(UNICODE) || defined(_UNICODE)
	localCopy.fnGetCurrentDirectory = (PGetCurrentDirectory)GetProcAddress( 
	      GetModuleHandle( _T("Kernel32.dll") ), "GetCurrentDirectoryA" );
#else
	localCopy.fnGetCurrentDirectory = (PGetCurrentDirectory)GetProcAddress( 
	      GetModuleHandle( _T("Kernel32.dll") ), "GetCurrentDirectoryA" );
#endif

	if (localCopy.fnGetCurrentDirectory )
		goto cleanup;
		
	// copy the parameterblock to the other process adress space
	if ( ! WriteProcessMemory( hProcess, c, &localCopy, sizeof localCopy, 0 ) )
		goto cleanup;

   // CreateRemoteThread()
	ht = CreateRemoteThread( hProcess, 0, 0, (DWORD (__stdcall *)( void *)) p, c, 0, &rc );
	if ( ht == NULL )
		goto cleanup;
	
	rc = WaitForSingleObject( ht, INFINITE );
	switch ( rc )
	{
	case WAIT_TIMEOUT:
		goto cleanup;

	case WAIT_FAILED:
		goto cleanup;

	case WAIT_OBJECT_0:
		// this might just have worked, pick up the result!
		// rad back the prameter block, it has the error code
		if ( ! ReadProcessMemory( hProcess, c, &localCopy, sizeof localCopy, 0 ) )
			goto cleanup;

      if ( pRetCode != NULL )
         *pRetCode = localCopy.dwReturnCode;

      if ( localCopy.dwReturnCode != 0 )
         _tcsncpy( lpPath, localCopy.lpDirectory, size );

      break;
	
	default:
		break;
	}

cleanup:
 	// Let's clean
   if ( ht != NULL )
	   CloseHandle( ht );
	if ( p != 0 )
		VirtualFreeEx( hProcess, p, 0, MEM_RELEASE );
	if ( c != 0 )
		VirtualFreeEx( hProcess, c, 0, MEM_RELEASE );
	if ( hProcess != NULL)
		CloseHandle( hProcess );

	return rc;
}

BOOL RemoteSimpleHandleFunction( DWORD processId, HANDLE handle, char* lpszFunction )
{
   HANDLE hRemoteThread;
   BOOL rc = 0;

 	//Enable debug privilege
	EnableDebugPriv();

	// open the process
	HANDLE hProcess = OpenProcess( PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |
			PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, processId );

	if ( hProcess == NULL )
	   return FALSE;

	hRemoteThread = CreateRemoteThread( 
	      hProcess, 
	      0, 
	      0, 
	      (DWORD (__stdcall *)( void *))GetProcAddress( 
	               GetModuleHandle(_T("KERNEL32.DLL")), lpszFunction ),
	      handle, 
	      0, 
	      (DWORD*)&rc );

	CloseHandle( hProcess );

	return rc;
}

// The whole shebang makes a number of assumptions:
// -- target process is a Win32 process
// -- kernel32.dll loaded at same address in each process (safe)
// -- bem() shorter than MAXINJECTSIZE
// -- bem() does not rely on the C/C++ runtime
// -- /GZ is _not_ used. (If it is, the compiler generates calls
//    to functions which are not injected into the target. Oops!
DWORD __stdcall RemoteDllThread( RemoteDllThreadBlock* execBlock )
{
	// and this is the code we are injecting

	typedef DWORD (*PRemoteDllFunction)();

	HMODULE hModule = NULL;

	// clear the error codes
	execBlock->ErrorLoad = 0;
	execBlock->ErrorFunction = 0;
	execBlock->ErrorFree = 0;

	// load the requested dll
	if ( execBlock->bLoadLibrary )
	{
		execBlock->hModule = (HMODULE)(*execBlock->fnLoadLibrary)( execBlock->lpModulePath );

		hModule = execBlock->hModule;

		execBlock->ErrorLoad = execBlock->hModule != NULL ? 0 : 1;
	}

	// if we didn't load the library, try to query the module handle
	if ( hModule == NULL )
		hModule = (*execBlock->fnGetModuleHandle)( execBlock->lpModulePath );

	// call function
	if ( execBlock->lpFunctionName[0] != 0 )
	{
		//execute a function if we have a function name
		PRemoteDllFunction fnRemoteDllFunction = (PRemoteDllFunction)
			(*execBlock->fnGetProcAddress)( hModule, execBlock->lpFunctionName );

		// execute the function, and get the result
		if ( fnRemoteDllFunction != NULL )
		{
			execBlock->ErrorFunction = 0;
			execBlock->ReturnCodeForFunction = (*fnRemoteDllFunction)();
		}
		else
			execBlock->ErrorFunction = 1;
	}

	// free library
	if ( execBlock->bFreeLibrary )
	{
		execBlock->ErrorFree = execBlock->fnFreeLibrary( hModule ) ? 0 : 1;
	}

	execBlock->hModule = hModule;
	
	return 0;
}

// and this is the code we are injecting
DWORD __stdcall RemoteGetCurrentDirectoryThread( RemoteGetCurrentDirectoryThreadBlock* pParam )
{
   pParam->dwReturnCode = (*pParam->fnGetCurrentDirectory )( pParam->lpDirectory, _MAX_PATH );

   return 0;
}