#ifndef LOADDLL_H_INCLUDED
#define LOADDLL_H_INCLUDED

//#include "TaskManagerExDll.h"

typedef HMODULE (WINAPI *PLoadLibrary)(LPCTSTR);
typedef HMODULE (WINAPI *PGetModuleHandle)(LPCTSTR);
typedef BOOL    (WINAPI *PFreeLibrary)(HMODULE);
typedef FARPROC (WINAPI *PGetProcAddress)(HMODULE, char*);
typedef DWORD   (WINAPI *PGetCurrentDirectory)(LPTSTR,DWORD);

struct RemoteDllThreadBlock
{
	DWORD				   ErrorLoad;					// error value for LoadLibrary
	DWORD				   ErrorFunction;				// error value for executed function
	DWORD				   ReturnCodeForFunction;	// error value for executed function
	DWORD				   ErrorFree;					// error value for FreeLibrary

	HMODULE			   hModule;
	BOOL				   bLoadLibrary;
	BOOL				   bFreeLibrary;

	PLoadLibrary		fnLoadLibrary;
	PGetModuleHandle	fnGetModuleHandle;
	PFreeLibrary		fnFreeLibrary;
	PGetProcAddress	fnGetProcAddress;

	TCHAR 			   lpModulePath[_MAX_PATH];	// the DLL path
	char				   lpFunctionName[256];		// the called function
};

struct RemoteGetCurrentDirectoryThreadBlock
{
	PGetCurrentDirectory	fnGetCurrentDirectory;
	TCHAR 			      lpDirectory[_MAX_PATH];
	DWORD                dwReturnCode;
};

// try to enable SeDebugPrivilege
void EnableDebugPriv( void );

// inject function RemoteThread() into target process
DWORD ExecuteRemoteThread( HANDLE, BOOL, BOOL, LPCTSTR, char*, DWORD*, DWORD*, DWORD*, DWORD* );

// and this is the code we are injecting
DWORD __stdcall RemoteDllThread( RemoteDllThreadBlock* );

// and this is the code we are injecting
DWORD __stdcall RemoteGetCurrentDirectoryThread( RemoteGetCurrentDirectoryThreadBlock* );

// That's the THING
DWORD LoadDllForRemoteThread( DWORD, BOOL, BOOL, LPCTSTR, char*, DWORD*, DWORD*, DWORD*, DWORD* );
DWORD RemoteGetCurrentDirectory( DWORD, LPTSTR, DWORD, DWORD* );

// Check OS
DWORD IsWindowsNT();

BOOL RemoteSimpleHandleFunction( DWORD processId, HANDLE handle, char* lpszFunction );

#endif