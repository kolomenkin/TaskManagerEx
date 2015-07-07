#ifndef LOADDLL_H_INCLUDED
#define LOADDLL_H_INCLUDED

//#include "TaskManagerExDll.h"

namespace RemoteExecute
{
	enum eLoad
	{
		LOAD_LIBRARY = TRUE,
		DONT_LOAD_LIBRARY = FALSE,
	};

	enum eFree
	{
		FREE_LIBRARY = TRUE,
		DONT_FREE_LIBRARY = FALSE,
	};

	enum eSpecialMode
	{
		NO_SPECIAL_MODE,
		GET_COMMAND_LINE,
		GET_ENVIRONMENT,
	};

	static const char* const NoLibrary = NULL;
	static const char* const NoFunction = NULL;
	static DWORD_PTR* const NoFuncReturn = NULL;
	static DWORD_PTR* const NoFuncReturnSpecial = NULL;
	static LONG*  const NoLastError = NULL;
	static DWORD* const NoErrorLoad = NULL;
	static DWORD* const NoErrorFunction = NULL;
	static DWORD* const NoErrorFree = NULL;
	static DWORD  ZeroArguments = 0;
	static DWORD* const NoArguments = NULL;

#define DONT_RETURN_REMOTE_API_ERRORS	NoErrorLoad, NoErrorFunction, NoErrorFree
}

typedef HMODULE (WINAPI *PLoadLibrary)(LPCTSTR);
typedef HMODULE (WINAPI *PGetModuleHandle)(LPCTSTR);
typedef BOOL    (WINAPI *PFreeLibrary)(HMODULE);
typedef FARPROC (WINAPI *PGetProcAddress)(HMODULE, char*);
//typedef DWORD   (WINAPI *PGetCurrentDirectory)(LPWSTR,DWORD);
typedef DWORD   (WINAPI *PGetLastError) ( VOID );
typedef VOID    (WINAPI *PSetLastError) ( DWORD dwErrCode );

#define MARKER_BEGIN	0x19822891
#define MARKER_END		0x31415926
#define REMOTE_MAX_ARGUMENTS	8

struct RemoteDllThreadBlock
{
	DWORD				MarkerBegin;
	DWORD				ErrorLoad;					// error value for LoadLibrary
	DWORD				ErrorFunction;				// error value for executed function
	DWORD_PTR			ReturnCodeForFunction;		// function return code
	RemoteExecute::eSpecialMode	SpecialMode;
	DWORD_PTR			ReturnCodeForFunctionSpecial;// Special Mode value that is calculated from ReturnCodeForFunction.
													// Usually this is the size of remote buffer pointed by ReturnCodeForFunction.
	DWORD				ErrorFree;					// error value for FreeLibrary
	DWORD				LastError;

	HMODULE				hModule;
	BOOL				bLoadLibrary;
	BOOL				bFreeLibrary;
	DWORD				dwArgumentCount; // 0..8
	DWORD				Arguments[REMOTE_MAX_ARGUMENTS];

	PLoadLibrary		fnLoadLibrary;
	PGetModuleHandle	fnGetModuleHandle;
	PFreeLibrary		fnFreeLibrary;
	PGetProcAddress		fnGetProcAddress;
	PGetLastError		fnGetLastError;
	PSetLastError		fnSetLastError;

	TCHAR				lpModulePath[MAX_PATH];	// the DLL path
	CHAR				lpFunctionName[256];		// the called function
	DWORD				MarkerEnd;
};

//struct RemoteGetCurrentDirectoryThreadBlock
//{
//	PGetCurrentDirectory	fnGetCurrentDirectory;
//	TCHAR					lpDirectory[MAX_PATH];
//	DWORD					dwReturnCode;
//};

// try to enable SeDebugPrivilege
//TASKMANAGEREXDLL_API void EnableDebugPriv( void );

// inject function RemoteThread() into target process
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
		DWORD*	pdwArguments
		);

// and this is the code we are injecting
DWORD __stdcall RemoteDllThread( RemoteDllThreadBlock* );

// and this is the code we are injecting
//DWORD __stdcall RemoteGetCurrentDirectoryThread( RemoteGetCurrentDirectoryThreadBlock* );

// That's the THING
// The whole shebang makes a number of assumptions:
// -- target process is a Win32 process
// -- kernel32.dll loaded at same address in each process (safe)
// -- bem() shorter than MAXINJECTSIZE
// -- bem() does not rely on the C/C++ runtime
// -- /GZ is _not_ used. (If it is, the compiler generates calls
//    to functions which are not injected into the target. Oops!
// -- Target function uses WINAPI (pascal) call convention.
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
		DWORD*	pdwArguments
		);

void FreeSpecialBuffer( DWORD pReturnCodeForFunction );

//DWORD RemoteGetCurrentDirectory( DWORD, LPWSTR, DWORD, DWORD* );

BOOL RemoteSimpleFunction( DWORD processId, DWORD dwArgument, char* lpszFunction, DWORD* lpdwFuncRetVal );

#endif