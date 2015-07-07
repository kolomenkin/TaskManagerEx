// Written by Zoltan Csizmadia, zoltan_csizmadia@yahoo.com
// For companies(Austin,TX): If you would like to get my resume, send an email.
//
// The source is free, but if you want to use it, mention my name and e-mail address
//
//////////////////////////////////////////////////////////////////////////////////////
//
// SystemInfo.h, v1.1

#ifndef SYSTEMINFO_H_INCLUDED
#define SYSTEMINFO_H_INCLUDED

//#ifndef WINNT
//#error You need Windows NT to use this source code. Define WINNT!
//#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )
#pragma warning( disable : 4200 )

#include <afxtempl.h>

#include "SystemInfoListCtrl.h"
#include "WindowsCore.h"
#include "VMQuery.h"
#include "WinObjects.h"

//////////////////////////////////////////////////////////////////////////////////////

#define PID_SYSTEM_WIN_NT4		2
#define PID_SYSTEM_WIN_2K		4
#define PID_SYSTEM_WIN_XP		8

#define IS_PID_SYSTEM(pid)		((pid) == PID_SYSTEM_WIN_NT4 || \
								 (pid) == PID_SYSTEM_WIN_2K || \
								 (pid) == PID_SYSTEM_WIN_XP )

#define ALL_PROCESSES	((DWORD)-1)

//////////////////////////////////////////////////////////////////////////////////////

BOOL beginsi( const CString& s, const CString& strBegin );

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemInfoUtils
//
//////////////////////////////////////////////////////////////////////////////////////

// Helper functions

class SystemInfoUtils
{
public:

	//////////////////////////////////////////////////////////////////////////////////
	// String conversion functions

	// From wide char string to CString
	static void LPCWSTR2CString( LPCWSTR strW, CString& str );
	// From unicode string to CString
	static void Unicode2CString( UNICODE_STRING* strU, CString& str );

	//////////////////////////////////////////////////////////////////////////////////
	// File name conversion functions

	static BOOL GetDeviceFileName( LPCTSTR, CString& );
	static BOOL GetFsFileName( LPCTSTR, CString& );

	static CString DecodeModuleName( const CString& strFullName );
};

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemProcessInformation
//
//////////////////////////////////////////////////////////////////////////////////////

class SystemProcessInformation : INtDll, IPsapi
{
public:

	struct PROCESS_INFO // short name because m_ProcessInfos produces too long identifier warning in debug configuration
	{
		DWORD	processId;

		SYSTEM_THREAD_INFORMATION*	pThreads;	// INtDll::NtQuerySystemInformation: SystemProcessInformation

		// PID, Parent PID, BasePriority, ProcessName, ftCreation, ftKernel, ftUser,
		// PROCESS_MEMORY_COUNTERS, IO_COUNTERS, ThreadCount, HandleCount:
		SYSTEM_PROCESS_INFORMATION	spi;		// INtDll::NtQuerySystemInformation: SystemProcessInformation

		// PID, Parent PID, BasePriority, AffinityMask, ExitStatus, PPEB:
		// PPEB (current process only???): debugger, sessionId, OSVersion, ImageSubSystem,
		//      HeapCount, HeapAddresses, NumberOfProcessors,
		//      LoaderData: ModuleLists (by load, by memory, by init), bInitialized
		//      ProcessParameters: CurrentDirectory, DllSearchPaths, ImagePath, CommandLine,
		//           pEnvironment, WindowTitle?, DesktopName,
		// etc, etc, etc...
		PROCESS_BASIC_INFORMATION	pi;			// INtDll::NtQueryInformationProcess: ProcessBasicInformation

		PROCESS_MEMORY_COUNTERS		pmc;		// IPsapi::GetProcessMemoryInfo, see also SYSTEM_PROCESS_INFORMATION
		//IO_COUNTERS				ioc;		// Windows 2000+ (GetProcessIoCounters), see also SYSTEM_PROCESS_INFORMATION

		BOOL		bDisablePriorityBoost;		// GetProcessPriorityBoost
		DWORD_PTR	nProcessAffinity;			// GetProcessAffinityMask
		DWORD_PTR	nSystemAffinity;			// GetProcessAffinityMask
		//DWORD		dwDefaultLayout;			// Windows 2000+ (GetProcessDefaultLayout)
		FILETIME	ftCreation;					// GetProcessTimes, see also SYSTEM_PROCESS_INFORMATION
		FILETIME	ftExit;						// GetProcessTimes
		FILETIME	ftKernel;					// GetProcessTimes, see also SYSTEM_PROCESS_INFORMATION
		FILETIME	ftUser;						// GetProcessTimes, see also SYSTEM_PROCESS_INFORMATION
		DWORD		dwVersion;					// GetProcessVersion
		SIZE_T		minWorkSet;					// GetProcessWorkingSetSize
		SIZE_T		maxWorkSet;					// GetProcessWorkingSetSize

		TCHAR		szExe[MAX_PATH];			// IPsapi::GetModuleFileNameEx( NULL )
	};

	enum { BufferSize = 0x10000 };

public:
	SystemProcessInformation( DWORD processId, BOOL bAdditionalInfo, BOOL bRefresh );
	virtual ~SystemProcessInformation();

	BOOL Refresh();

	static BOOL GetAdditionalInfo( PROCESS_INFO& info );

public:
	CMap< DWORD, DWORD&, PROCESS_INFO, PROCESS_INFO&> m_ProcessInfos;

protected:
	DWORD		m_processId;
	BOOL		m_bAdditionalInfo;
	UCHAR*		m_pBuffer;
};

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemThreadInformation
//
//////////////////////////////////////////////////////////////////////////////////////

class SystemThreadInformation : INtDll, IPsapi
{
public:

	struct THREAD_INFORMATION
	{
		//DWORD		ThreadId;
		DWORD		ProcessId;
		HANDLE		ThreadHandle;
		DWORD		HandleProcessId;
		//SYSTEM_THREAD st;
		SYSTEM_THREAD_INFORMATION sti;
		//THREADENTRY32	te;
		TCHAR		Module[MAX_PATH];

		static void InsertColumns( CSystemInfoListCtrl& list, BOOL bPid );
		int Insert( CSystemInfoListCtrl& list, BOOL bPid, int iItem, int iItemCount ) const;
	};

public:
	SystemThreadInformation( DWORD pID, BOOL bRefresh );

	BOOL Refresh();

	// Returns the HMODULE that contains the specified memory address
	static BOOL ModuleFromAddressEx( DWORD processId, PVOID pv, LPTSTR szModuleName, int cbSize );

public:
	CList< THREAD_INFORMATION, THREAD_INFORMATION& > m_ThreadInfos;
	DWORD m_processId;
};


//////////////////////////////////////////////////////////////////////////////////////
//
// SystemHandleInformation
//
//////////////////////////////////////////////////////////////////////////////////////

class SystemHandleInformation : INtDll
{
public:

	struct HANDLE_INFORMATION
	{
		SYSTEM_HANDLE sh;

		static void InsertColumns( CSystemInfoListCtrl& list, BOOL bPid );
		int Insert( CSystemInfoListCtrl& list, BOOL bPid, int iItem, int iItemCount ) const;

		static void InsertFileColumns( CSystemInfoListCtrl& list, BOOL bPid );
		int InsertFile( CSystemInfoListCtrl& list, BOOL bPid, int iItem, int iItemCount,
			LPCTSTR szDevice, LPCTSTR szPath ) const;
	};

protected:
	typedef struct _GetFileNameThreadParam
	{
		HANDLE		hFile;
		CString*	pName;
		NTSTATUS	rc;
	} GetFileNameThreadParam;

public:
	SystemHandleInformation( DWORD pID, BOOL bRefresh, LPCTSTR lpTypeFilter /* = NULL*/ );
	~SystemHandleInformation();

	BOOL SetFilter( LPCTSTR lpTypeFilter, BOOL bRefresh = TRUE );
	const CString& GetFilter();

	BOOL Refresh();

public:
	//Information functions
	static BOOL GetType( HANDLE, OB_TYPE_ENUM& type, DWORD processId = GetCurrentProcessId() );
	static BOOL GetTypeToken( HANDLE, CString&, DWORD processId = GetCurrentProcessId() );
	static BOOL GetNameByType( HANDLE, OB_TYPE_ENUM type, CString& str, DWORD processId = GetCurrentProcessId());
	static BOOL GetName( HANDLE, CString&, DWORD processId = GetCurrentProcessId() );

	//Thread related functions
	static BOOL GetThreadId( HANDLE, DWORD&, DWORD processId = GetCurrentProcessId() );

	//Process related functions
	static BOOL GetProcessId( HANDLE, DWORD&, DWORD processId = GetCurrentProcessId() );
	static BOOL GetProcessPath( HANDLE h, CString& strPath, DWORD processId = GetCurrentProcessId());

	//File related functions
	static BOOL GetFileName( HANDLE, CString&, DWORD processId = GetCurrentProcessId() );

public:
	//For remote handle support
	static HANDLE OpenProcess( DWORD processId );
	static HANDLE DuplicateHandle( HANDLE hProcess, HANDLE hRemote );

	static HANDLE DuplicateHandle( DWORD processId, HANDLE hRemote )
	{
		HANDLE hLocal = NULL;
		HANDLE hProcess = OpenProcess( processId );
		if( hProcess != NULL )
		{
			hLocal = DuplicateHandle( hProcess, hRemote );
			CloseHandle( hProcess );
		}
		return hLocal;
	}

protected:
	static UINT __stdcall GetFileNameThread( PVOID /* GetFileNameThreadParam* */ );
	BOOL IsSupportedHandle( SYSTEM_HANDLE& handle );

public:
	CList< HANDLE_INFORMATION, HANDLE_INFORMATION& > m_HandleInfos;
	DWORD	m_processId;

protected:
	CString	m_strTypeFilter;
};

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemModuleInformation
//
//////////////////////////////////////////////////////////////////////////////////////

class SystemModuleInformation: IPsapi, IToolhelp32
{
public:

	typedef struct _MODULE_INFO
	{
		DWORD	ProcessId;
		TCHAR	FullPath[MAX_PATH];		// module full name
		TCHAR	FileFullPath[MAX_PATH];	// module real file name
		TCHAR	Name[MAX_PATH];
		HMODULE Handle;
		MODULEINFO info;
		MEMORY_BASIC_INFORMATION mbi;
		MODULEENTRY32 me32;

		static void InsertColumns( CSystemInfoListCtrl& list, BOOL bPid );
		int Insert( CSystemInfoListCtrl& list, BOOL bPid, int iItem, int iItemCount ) const;
	} MODULE_INFO;

public:
	SystemModuleInformation( DWORD pID, BOOL bRefresh );

	BOOL Refresh();

protected:
	void GetModuleListForProcess( DWORD processID );

public:
	DWORD m_processId;
	CList< MODULE_INFO, MODULE_INFO& > m_ModuleInfos;

protected:
};

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemKernelModuleInformation
//
//////////////////////////////////////////////////////////////////////////////////////

class SystemKernelModuleInformation: IPsapi
{
public:

	struct KERNEL_MODULE_INFORMATION
	{
		LPVOID	pBaseAddress;
		TCHAR	FullPath[MAX_PATH];
		TCHAR	Name[MAX_PATH];

		static void InsertColumns( CSystemInfoListCtrl& list );
		int Insert( CSystemInfoListCtrl& list, int iItem, int iItemCount ) const;
	};

public:
	SystemKernelModuleInformation( BOOL bRefresh );

	BOOL Refresh();

public:
	CList< KERNEL_MODULE_INFORMATION, KERNEL_MODULE_INFORMATION& > m_KernelModuleInfos;
};


//////////////////////////////////////////////////////////////////////////////////////
//
// SystemMemoryMapInformation
//
//////////////////////////////////////////////////////////////////////////////////////

class SystemMemoryMapInformation: IPsapi
{
public:

	struct MEMORY_INFORMATION
	{
		BOOL		bRegion; // else block, part of region
		VMQUERY		vmq;
		TCHAR		Module[MAX_PATH];
		TCHAR		MappedFile[MAX_PATH];

		static void InsertColumns( CSystemInfoListCtrl& list );
		int Insert( CSystemInfoListCtrl& list, int iItem, int iItemCount, BOOL bExpandRegions ) const;
	};

public:
	SystemMemoryMapInformation( DWORD pID, BOOL bExpandRegions, BOOL bRefresh );

	static BOOL FileFromAddress( DWORD processId, PVOID pv, LPTSTR szFileName, int cbSize );
	BOOL Refresh( BOOL bExpandRegions );

	BOOL AddMemoryInfo( DWORD processId, HANDLE hProcess, BOOL bRegion, PVMQUERY pVmq );
public:
	DWORD m_processId;
	CList< MEMORY_INFORMATION, MEMORY_INFORMATION& > m_MemoryInfos;
};


//////////////////////////////////////////////////////////////////////////////////////
//
// SystemWindowInformation
//
//////////////////////////////////////////////////////////////////////////////////////

class SystemWindowInformation
{
public:
	enum { MaxCaptionSize = 1024 };

	typedef struct _WINDOW_INFO
	{
		DWORD	ProcessId;
		TCHAR	Caption[MaxCaptionSize];
		HWND	hWnd;
	} WINDOW_INFO;

public:
	SystemWindowInformation( DWORD pID, BOOL bRefresh );

	BOOL Refresh();

protected:
	static BOOL CALLBACK EnumerateWindows( HWND hwnd, LPARAM lParam );

public:
	DWORD m_processId;
	CList< WINDOW_INFO, WINDOW_INFO& > m_WindowInfos;
};

//////////////////////////////////////////////////////////////////////////////////////

#endif
