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
#include "../TaskManagerExDll/TaskManagerExDllExport.h" // for TASKMANAGEREXDLL_DEBUG_API macro

#include "WindowsCore.h"

//------------------------------------------------------

#ifdef _WIN64
const DWORD MAXINJECTSIZE = 8192;
#else
const DWORD MAXINJECTSIZE = 4096;
#endif

void RemoteDllThread_NextProc();

//------------------------------------------------------

PVOID GetFuncAddress(PVOID addr)
{
#ifdef _DEBUG
	//check if instruction is relative jump (E9)
	if (0xE9 != *((UCHAR*)addr))
		return addr;

	// calculate base of relative jump
	const ULONG_PTR base = (ULONG_PTR)((UCHAR*)addr + 5);

	// calculate offset 
	const LONG *offset = (LONG*)((UCHAR*)addr + 1);

	return (PVOID)(base + (LONG_PTR)*offset);
#else
	// in release, don't have to mess with jumps
	return addr;
#endif
}


DWORD LoadDllForRemoteThread(
		DWORD	processID,
		RemoteExecute::eLoad	bLoad,
		RemoteExecute::eFree	bFree,
		RemoteExecute::eSpecialMode SpecialMode,
		LPCTSTR	lpModuleName,
		LPCSTR	lpFunctionName,
		DWORD_PTR*	pReturnCodeForFunction,
		DWORD_PTR*	pReturnCodeForFunctionSpecial,
		LONG*	pLastError,
		DWORD*	pErrorLoad,
		DWORD*	pErrorFunction,
		DWORD*	pErrorFree,
		DWORD	dwArgumentCount,
		DWORD_PTR*	pdwArguments
		)
{
	DWORD rc = (DWORD)-2;

	//Enable debug privilege
	EnableDebugPriv();

	// open the process
	HANDLE hProcess = OpenProcess( PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |
			PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, processID );

	if ( hProcess != NULL )
	{
		// let's work
		rc = ExecuteRemoteThread( hProcess, bLoad, bFree, SpecialMode, lpModuleName, lpFunctionName,
					pReturnCodeForFunction, pReturnCodeForFunctionSpecial, pLastError,
					pErrorLoad, pErrorFunction, pErrorFree,
					dwArgumentCount, pdwArguments );

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

DWORD ExecuteRemoteThread(
		HANDLE	hProcess,
		RemoteExecute::eLoad	bLoad,
		RemoteExecute::eFree	bFree,
		RemoteExecute::eSpecialMode SpecialMode,
		LPCTSTR	lpModuleName,
		LPCSTR	lpFunctionName,
		DWORD_PTR*	pReturnCodeForFunction,
		DWORD_PTR*	pReturnCodeForFunctionSpecial,
		LONG*	pLastError,
		DWORD*	pErrorLoad,
		DWORD*	pErrorFunction,
		DWORD*	pErrorFree,
		DWORD	dwArgumentCount,
		DWORD_PTR*	pdwArguments
		)
{
	HANDLE ht = 0;
	void *p = 0;
	RemoteDllThreadBlock *c = 0;
	DWORD rc = (DWORD)-1;
	HMODULE hKernel32 = 0;
	RemoteDllThreadBlock localCopy;
	DWORD i = 0;
	DWORD ThreadId = 0;
	SIZE_T nReadBytes = 0;
	DWORD WaitObjectRes = 0;
	BYTE* pSpecialBuffer = NULL;

	// clear the parameter block
	//my_memset( &localCopy, 0, sizeof(localCopy) );
	my_memset( &localCopy, 0xAA, sizeof(localCopy) ); // debug purposes

	// allocate memory for injected code
	p = VirtualAllocEx( hProcess, 0, MAXINJECTSIZE, MEM_COMMIT, PAGE_EXECUTE_READWRITE );
	if ( p == 0 )
	{
		rc = (DWORD)-101;
		TRACE(_T("ExecuteRemoteThread: VirtualAllocEx failed: %d\n"), GetLastError());
		goto cleanup;
	}

	// allocate memory for parameter block
	c = (RemoteDllThreadBlock*) VirtualAllocEx( hProcess, 0, sizeof(RemoteDllThreadBlock), MEM_COMMIT, PAGE_READWRITE );
	if ( c == 0 )
	{
		rc = (DWORD)-102;
		goto cleanup;
	}

	const ptrdiff_t nFuncSize = SIZE_T(PBYTE(GetFuncAddress(RemoteDllThread_NextProc)) - PBYTE(GetFuncAddress(RemoteDllThread)));
	if (g_bMoreLogging)
	{
		TRACE(_T("ExecuteRemoteThread: RemoteDllThread function size: %Id\n"), nFuncSize);
	}
	ASSERT(MAXINJECTSIZE >= nFuncSize);
	RemoteDllThread_NextProc();	// it does nothing, put here just to prevent optimizers to do any optimization

	// copy function there, we will execute this piece of code
	if ( ! WriteProcessMemory( hProcess, p, GetFuncAddress(RemoteDllThread), MAXINJECTSIZE, 0 ) )
	{
		rc = (DWORD)-103;
		TRACE(_T("ExecuteRemoteThread: WriteProcessMemory failed: %d\n"), GetLastError());
		goto cleanup;
	}

	// copy dll path to parameter block
	lstrcpyn( localCopy.lpModulePath, lpModuleName, SIZEOF_ARRAY(localCopy.lpModulePath) );
//#ifdef _UNICODE
//	lstrcpyW( localCopy.lpModulePath, lpModuleName );
//#else
//	wsprintfW( localCopy.lpModulePath, L"%hs", lpModuleName );
//#endif

	if ( lpFunctionName == NULL )
		localCopy.lpFunctionName[0] = '\0';
	else
		lstrcpynA( localCopy.lpFunctionName, lpFunctionName, SIZEOF_ARRAY(localCopy.lpFunctionName) );
	
	localCopy.bLoadLibrary = bLoad;
	localCopy.bFreeLibrary = bFree;
	localCopy.SpecialMode = SpecialMode;
	localCopy.MarkerBegin = MARKER_BEGIN;
	localCopy.MarkerEnd = MARKER_END;

	// kernel32.dll
	hKernel32 = GetModuleHandle( _T("kernel32.dll") );

	if ( hKernel32 == NULL )
	{
		rc = (DWORD)-104;
		goto cleanup;
	}

	// get the addresses for the functions, what we will use in the remote thread

	localCopy.fnLoadLibrary = (PLoadLibrary)GetProcAddress( hKernel32, "LoadLibrary" FUNC_SUFFIX );
	localCopy.fnGetModuleHandle = (PGetModuleHandle)GetProcAddress( hKernel32, "GetModuleHandle" FUNC_SUFFIX );
	localCopy.fnFreeLibrary = (PFreeLibrary)GetProcAddress( hKernel32, "FreeLibrary" );
	localCopy.fnGetProcAddress = (PGetProcAddress)GetProcAddress( hKernel32, "GetProcAddress" );
	localCopy.fnGetLastError = (PGetLastError)GetProcAddress( hKernel32, "GetLastError" );
	localCopy.fnSetLastError = (PSetLastError)GetProcAddress( hKernel32, "SetLastError" );

	if (localCopy.fnLoadLibrary == NULL ||
		localCopy.fnGetModuleHandle == NULL ||
		localCopy.fnFreeLibrary == NULL ||
		localCopy.fnGetProcAddress == NULL)
		goto cleanup;

	if( dwArgumentCount > REMOTE_MAX_ARGUMENTS )
	{
		rc = (DWORD)-105;
		goto cleanup;
	}

	if( pdwArguments == NULL && dwArgumentCount != 0 )
	{
		rc = (DWORD)-106;
		goto cleanup;
	}

	localCopy.dwArgumentCount = dwArgumentCount;
	for( i=0; i<dwArgumentCount; i++ )
	{
		localCopy.Arguments[i] = pdwArguments[i];
	}

	// copy the parameterblock to the other process adress space
	if ( ! WriteProcessMemory( hProcess, c, &localCopy, sizeof localCopy, 0 ) )
	{
		rc = (DWORD)-107;
		goto cleanup;
	}

	// CreateRemoteThread()
	ht = CreateRemoteThread( hProcess, 0, 0, (DWORD (__stdcall *)( void *)) p, c, 0, &ThreadId );
	if ( ht == NULL )
	{
		rc = (DWORD)-108;
		TRACE(_T("ExecuteRemoteThread: CreateRemoteThread failed: %d\n"), GetLastError());
		goto cleanup;
	}

	WaitObjectRes = WaitForSingleObject( ht, INFINITE );
	switch ( WaitObjectRes )
	{
	case WAIT_TIMEOUT:
	{
		rc = (DWORD)-109;
		goto cleanup;
	}

	case WAIT_FAILED:
	{
		rc = (DWORD)-110;
		goto cleanup;
	}

	case WAIT_OBJECT_0:
		// this might just have worked, pick up the result!
		// rad back the parameter block, it has the error code

		// clear the parameter block
		my_memset( &localCopy, 0, sizeof(localCopy) );

		if ( ! ReadProcessMemory( hProcess, c, &localCopy, sizeof(localCopy), &nReadBytes ) )
		{
			rc = 0x100;
			goto cleanup;
		}

		if( nReadBytes != sizeof(localCopy) )
		{
			rc = 0x200;
			goto cleanup;
		}

		if( localCopy.MarkerBegin != MARKER_BEGIN )
		{
			rc = 0x300;
			goto cleanup;
		}

		if( localCopy.MarkerEnd != MARKER_END )
		{
			rc = 0x400;
			goto cleanup;
		}

      rc = 0;

      if ( pReturnCodeForFunction != NULL )
         *pReturnCodeForFunction = localCopy.ReturnCodeForFunction;

	  if( pLastError != NULL )
		 *pLastError = localCopy.LastError;

      if ( pErrorLoad != NULL )
         *pErrorLoad = localCopy.ErrorLoad;

      if ( pErrorFunction != NULL )
         *pErrorFunction = localCopy.ErrorFunction;

      if ( pErrorFree != NULL )
         *pErrorFree = localCopy.ErrorFree;


		if( SpecialMode != RemoteExecute::NO_SPECIAL_MODE )
		{
			if ( pReturnCodeForFunction != NULL )
				*pReturnCodeForFunction = 0;

			if ( pReturnCodeForFunctionSpecial != NULL )
				*pReturnCodeForFunctionSpecial = 0;

			if( pReturnCodeForFunction != NULL
				&& pReturnCodeForFunctionSpecial != NULL )
			{
				LPCVOID pRemoteBuffer = (LPCVOID) localCopy.ReturnCodeForFunction;
				if( pRemoteBuffer != NULL )
				{
					size_t n = localCopy.ReturnCodeForFunctionSpecial;
					pSpecialBuffer = (PBYTE) LocalAlloc( LMEM_FIXED, n );
					if( pSpecialBuffer != NULL )
					{
						my_memset( pSpecialBuffer, 0, n );
						if ( ! ReadProcessMemory( hProcess, pRemoteBuffer, pSpecialBuffer, n, &nReadBytes ) )
						{
							rc = 0x500;
							goto cleanup;
						}

						if( nReadBytes != n )
						{
							rc = 0x600;
							goto cleanup;
						}

						*pReturnCodeForFunction = (DWORD_PTR) pSpecialBuffer;
						*pReturnCodeForFunctionSpecial = n;
						pSpecialBuffer = NULL;
					}
				}
			}
		}

      if ( localCopy.ErrorLoad )
         rc |= 1;

      if ( localCopy.ErrorFunction == 1 )
         rc |= 2;

      if ( localCopy.ErrorFree )
         rc |= 4;

      if ( localCopy.ErrorFunction == 2 )
         rc |= 8;

		break;
	
	default:
		goto cleanup;
	}

cleanup:
	if( ht != NULL )
	{
		CloseHandle( ht );
	}

	if( pSpecialBuffer != NULL )
	{
		LocalFree( pSpecialBuffer );
	}

	// Let's clean
	if ( p != 0 )
		VirtualFreeEx( hProcess, p, 0, MEM_RELEASE );
	if ( c != 0 )
		VirtualFreeEx( hProcess, c, 0, MEM_RELEASE );
	
	return rc;
}

void FreeSpecialBuffer(DWORD_PTR pReturnCodeForFunction)
{
	if( pReturnCodeForFunction )
	{
		PBYTE p = (PBYTE)pReturnCodeForFunction;
		LocalFree( p );
	}
}


/*
DWORD RemoteGetCurrentDirectory( DWORD pID, LPWSTR lpPath, DWORD size, DWORD* pRetCode )
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
	my_memset( &localCopy, 0, sizeof(localCopy) );

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
	
	localCopy.fnGetCurrentDirectoryW = (PGetCurrentDirectoryW)GetProcAddress( 
	      GetModuleHandle( _T("kernel32.dll") ), "GetCurrentDirectoryW" );

	if (localCopy.fnGetCurrentDirectoryW == NULL)
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
	  {
         lstrcpynW( lpPath, localCopy.lpDirectory, size );
	  }

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
*/

BOOL RemoteSimpleFunction( DWORD processId, DWORD dwArgument, char* lpszFunction, DWORD* lpdwFuncRetVal )
{
	HANDLE hRemoteThread = NULL;
	BOOL rc = FALSE;
	HMODULE hKernel32 = NULL;
	FARPROC pfnProc = NULL;
	DWORD ThreadId = 0;
	DWORD dwWaitRes;

	//Enable debug privilege
	EnableDebugPriv();

	// open the process
	HANDLE hProcess = OpenProcess(
		PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
		FALSE, processId );

	if ( hProcess == NULL )
		goto cleanup;

	hKernel32 = GetModuleHandle(_T("kernel32.dll"));

	if ( hKernel32 == NULL )
		goto cleanup;

	pfnProc = GetProcAddress( hKernel32, lpszFunction );

	if ( pfnProc == NULL )
		goto cleanup;

	hRemoteThread = CreateRemoteThread( hProcess, 0, 0, 
		(LPTHREAD_START_ROUTINE)pfnProc, (LPVOID)dwArgument, 0, (DWORD*)&ThreadId );

	if( lpdwFuncRetVal != NULL )
	{
		dwWaitRes = WaitForSingleObject( hRemoteThread, INFINITE );
		switch ( rc )
		{
		case WAIT_OBJECT_0:
			GetExitCodeThread( hRemoteThread, lpdwFuncRetVal );
			break;

		case WAIT_FAILED:
		case WAIT_TIMEOUT:
		default:
			goto cleanup;
		}
	}

	rc = TRUE;

cleanup:

	if( hProcess != NULL )
	{
		CloseHandle( hProcess );
	}

	if( hRemoteThread != NULL )
	{
		CloseHandle( hRemoteThread );
	}

	return rc;
}

// The whole shebang makes a number of assumptions:
// -- target process is a Win32 process
// -- kernel32.dll loaded at same address in each process (safe)
// -- bem() shorter than MAXINJECTSIZE
// -- bem() does not rely on the C/C++ runtime
// -- /GZ is _not_ used. (If it is, the compiler generates calls
//    to functions which are not injected into the target. Oops!
// -- Target function uses WINAPI (pascal) call convention.
DWORD __stdcall RemoteDllThread( RemoteDllThreadBlock* execBlock )
{
	// and this is the code we are injecting

	typedef DWORD (WINAPI *PFN)();
	typedef DWORD (WINAPI *PFN1)(DWORD_PTR);
	typedef DWORD (WINAPI *PFN2)(DWORD_PTR, DWORD_PTR);
	typedef DWORD (WINAPI *PFN3)(DWORD_PTR, DWORD_PTR, DWORD_PTR);
	typedef DWORD (WINAPI *PFN4)(DWORD_PTR, DWORD_PTR, DWORD_PTR, DWORD_PTR);
	typedef DWORD (WINAPI *PFN5)(DWORD_PTR, DWORD_PTR, DWORD_PTR, DWORD_PTR, DWORD_PTR);
	typedef DWORD (WINAPI *PFN6)(DWORD_PTR, DWORD_PTR, DWORD_PTR, DWORD_PTR, DWORD_PTR, DWORD_PTR);
	typedef DWORD (WINAPI *PFN7)(DWORD_PTR, DWORD_PTR, DWORD_PTR, DWORD_PTR, DWORD_PTR, DWORD_PTR, DWORD_PTR);
	typedef DWORD (WINAPI *PFN8)(DWORD_PTR, DWORD_PTR, DWORD_PTR, DWORD_PTR, DWORD_PTR, DWORD_PTR, DWORD_PTR, DWORD_PTR);

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
		PFN pfn = (PFN)
			(*execBlock->fnGetProcAddress)( hModule, execBlock->lpFunctionName );

		// execute the function, and get the result
		if ( pfn != NULL )
		{
			DWORD_PTR ret = 0;
			DWORD_PTR* p = execBlock->Arguments;
			execBlock->ErrorFunction = 0;

			execBlock->LastError = 0;
			execBlock->fnSetLastError( execBlock->LastError );

			// !!!! Switch is a bad idea: compiler does something with it and
			// function don't copy correctly into the remote process
/*
#define S_BEGIN \
			switch( execBlock->dwArgumentCount ) \
			{ \

#define S_CASE(x)	case x:
#define S_ENDCASE	break;
// this condition must be checked and prevented in ExecuteRemoteThread()!
#define S_END \
				default: \
					execBlock->ErrorFunction = 1; \
			}
*/

#define S_BEGIN		DWORD cnt = execBlock->dwArgumentCount; if( cnt != execBlock->dwArgumentCount ) { }
#define S_CASE(x)	else if( cnt == (x) ) {
#define S_ENDCASE	}
#define S_END		else { execBlock->ErrorFunction = 2; }

			S_BEGIN
				S_CASE(0)		ret = (*(PFN) pfn)();  S_ENDCASE
				S_CASE(1)		ret = (*(PFN1)pfn)(p[0]);  S_ENDCASE
				S_CASE(2)		ret = (*(PFN2)pfn)(p[0],p[1]);  S_ENDCASE
				S_CASE(3)		ret = (*(PFN3)pfn)(p[0],p[1],p[2]);  S_ENDCASE
				S_CASE(4)		ret = (*(PFN4)pfn)(p[0],p[1],p[2],p[3]);  S_ENDCASE
				S_CASE(5)		ret = (*(PFN5)pfn)(p[0],p[1],p[2],p[3],p[4]);  S_ENDCASE
				S_CASE(6)		ret = (*(PFN6)pfn)(p[0],p[1],p[2],p[3],p[4],p[5]);  S_ENDCASE
				S_CASE(7)		ret = (*(PFN7)pfn)(p[0],p[1],p[2],p[3],p[4],p[5],p[6]);  S_ENDCASE
				S_CASE(8)		ret = (*(PFN8)pfn)(p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);  S_ENDCASE
			S_END

			execBlock->LastError = execBlock->fnGetLastError();
			execBlock->ReturnCodeForFunction = ret;

			if( execBlock->SpecialMode != RemoteExecute::NO_SPECIAL_MODE )
			{
				execBlock->ReturnCodeForFunctionSpecial = 0;
			}

			if( execBlock->SpecialMode == RemoteExecute::GET_COMMAND_LINE )
			{
				TCHAR* p = (TCHAR*)ret;
				if( p != NULL )
				{
					size_t n = 0;
					while( *p )
					{
						n++;
						p++;
					}
					execBlock->ReturnCodeForFunctionSpecial = (n + 1 ) * sizeof(TCHAR);
				}
			}
			else if( execBlock->SpecialMode == RemoteExecute::GET_ENVIRONMENT )
			{
				TCHAR* p = (TCHAR*)ret;
				if( p != NULL )
				{
					size_t n = 0;
					while( *p )
					{
						while( *p )
						{
							n++;
							p++;
						}
						n++;
						p++;
					}
					execBlock->ReturnCodeForFunctionSpecial = (n + 1 ) * sizeof(TCHAR);
				}

			}
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

void RemoteDllThread_NextProc()
{
}

/*
// and this is the code we are injecting
DWORD __stdcall RemoteGetCurrentDirectoryThread( RemoteGetCurrentDirectoryThreadBlock* pParam )
{
	pParam->dwReturnCode = (*pParam->fnGetCurrentDirectoryW )( pParam->lpDirectory, SIZEOF_ARRAY(pParam->lpDirectory) );

	return 0;
}
*/
