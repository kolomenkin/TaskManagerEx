//////////////////////////////////////////////////////////////
// File:		// WindowsCore.h
// File time:	// 30.03.2005	1:09
// Version: 	// 1.0
// Author:		// Sergey Kolomenkin <register@tut.by>
// Copyright:	// (c) Sergey Kolomenkin, Minsk, 2005
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#ifndef _WINDOWSCORE_H_UID0000002C4BF67042
#define _WINDOWSCORE_H_UID0000002C4BF67042

//////////////////////////////////////////////////////////////////////////////////////

#include "WindowsDdk.h"

//////////////////////////////////////////////////////////////////////////

#ifdef UNICODE
#	define FUNC_SUFFIX		"W"
#	define FUNC_SUFFIX_ALT	"W"
#else
#	define FUNC_SUFFIX		"A"
#	define FUNC_SUFFIX_ALT	""
#endif

//////////////////////////////////////////////////////////////////////////////////////

#define PROTECT_INTERFACES

//////////////////////////////////////////////////////////////////////////////////////
//
// INtDll
//
//////////////////////////////////////////////////////////////////////////////////////

class INtDll
{

#ifdef PROTECT_INTERFACES
	friend NTSTATUS MyNtQueryObject(
			IN	HANDLE				ObjectHandle OPTIONAL,
			IN	DWORD				ObjectInformationClass,
			OUT	PVOID				ObjectInformation,
			IN	ULONG				Length,
			OUT	PULONG				ResultLength );
	protected:
#else
	public:
#endif // ifdef PROTECT_INTERFACES


	typedef enum _SYSTEM_INFORMATION_CLASS {
		SystemBasicInformation = 0,
		SystemPerformanceInformation = 2,
		SystemTimeOfDayInformation = 3,
		//SystemProcessInformation = 5,
		_SystemProcessInformation = 5,
		SystemProcessorPerformanceInformation = 8,
		//SystemHandleInformation = 16, // ???????
		_SystemHandleInformation = 16, // ???????
		SystemInterruptInformation = 23,
		SystemExceptionInformation = 33,
		SystemRegistryQuotaInformation = 37,
		SystemLookasideInformation = 45,
		SystemExtendedHandleInformation = 0x40,
	} SYSTEM_INFORMATION_CLASS;

	typedef LARGE_INTEGER   QWORD;

	//////////////////////////////////////////////////////////////////
	// Process Basic Information: (NtQueryInformationProcess)

	typedef enum _PROCESS_INFORMATION_CLASS {
		ProcessBasicInformation = 0,
	} PROCESS_INFORMATION_CLASS;

	typedef struct _PROCESS_BASIC_INFORMATION {
			DWORD_PTR	ExitStatus;
			PPEB		PebBaseAddress;
			DWORD_PTR	AffinityMask;
			DWORD_PTR	BasePriority;
			DWORD_PTR	UniqueProcessId;
			DWORD_PTR	InheritedFromUniqueProcessId;

			DWORD GetPid() const
			{
				return (DWORD)UniqueProcessId;
			}

		} PROCESS_BASIC_INFORMATION;

	//////////////////////////////////////////////////////////////////
	// Process Information: (SystemProcessInformation)

	typedef struct _SYSTEM_THREAD_INFORMATION {
		LARGE_INTEGER KernelTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER CreateTime;
		ULONG         WaitTime;
		PVOID         StartAddress;
		CLIENT_ID     ClientId;
		KPRIORITY     Priority;
		KPRIORITY     BasePriority;
		ULONG         ContextSwitchCount;
		LONG          State;
		LONG          WaitReason;
	} SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;
/*
	typedef struct _SYSTEM_THREAD // NT4 ???
		{
		DWORD        u1;
		DWORD        u2;
		DWORD        u3;
		DWORD        u4;
		DWORD        ProcessId;
		DWORD        ThreadId;
		DWORD        dPriority;
		DWORD        dBasePriority;
		DWORD        dContextSwitches;
		DWORD        dThreadState;      // 2=running, 5=waiting
		DWORD        WaitReason;
		DWORD        u5;
		DWORD        u6;
		DWORD        u7;
		DWORD        u8;
		DWORD        u9;
		} SYSTEM_THREAD;
*/

/*
	typedef struct _IO_COUNTERS {
		ULONGLONG ReadOperationCount;
		ULONGLONG WriteOperationCount;
		ULONGLONG OtherOperationCount;
		ULONGLONG ReadTransferCount;
		ULONGLONG WriteTransferCount;
		ULONGLONG OtherTransferCount;
	} IO_COUNTERS, *PIO_COUNTERS;
//*/
	
	typedef struct _SYSTEM_PROCESS_INFORMATION
		{
			ULONG             NextEntryDelta;
			ULONG             ThreadCount;
//*
			ULONG             Reserved1[6];
			LARGE_INTEGER     CreateTime;
			LARGE_INTEGER     UserTime;
			LARGE_INTEGER     KernelTime;
			UNICODE_STRING    ProcessName;
			KPRIORITY         BasePriority;
			ULONG_PTR         ProcessId;
			ULONG_PTR         InheritedFromProcessId;
			ULONG             HandleCount;
			ULONG             Reserved2[2];
			VM_COUNTERS       VmCounters;
			// #if _WIN32_WINNT >= 0x500
			IO_COUNTERS       IoCounters;	// WARNING! If you get error that IO_COUNTERS is undeclared
											// then undefine it's definition above.
											// Latest SDK defines this structure in <WinNT.h>
			// #endif
//*/
/*
	DWORD dwUnknown1[6];
	FILETIME ftCreationTime;
	DWORD dwUnknown2[5];
	WCHAR* pszProcessName;
	DWORD dwBasePriority;
	DWORD ProcessId;
	DWORD dwParentProcessID;
	DWORD dwHandleCount;
	DWORD dwUnknown3;
	DWORD dwUnknown4;
	DWORD dwVirtualBytesPeak;
	DWORD dwVirtualBytes;
	DWORD dwPageFaults;
	DWORD dwWorkingSetPeak;
	DWORD dwWorkingSet;
	DWORD dwUnknown5;
	DWORD dwPagedPool;
	DWORD dwUnknown6;
	DWORD dwNonPagedPool;
	DWORD dwPageFileBytesPeak;
	DWORD dwPrivateBytes;
	DWORD dwPageFileBytes;
	DWORD dwUnknown7[4];
//*/
			SYSTEM_THREAD_INFORMATION Threads[1];
/*
		DWORD          dNext;
		DWORD          dThreadCount;
		DWORD          dReserved01;
		DWORD          dReserved02;
		DWORD          dReserved03;
		DWORD          dReserved04;
		DWORD          dReserved05;
		DWORD          dReserved06;
		QWORD          qCreateTime;
		QWORD          qUserTime;
		QWORD          qKernelTime;
		UNICODE_STRING usName;
		DWORD	       BasePriority;
		DWORD          dUniqueProcessId;
		DWORD          dInheritedFromUniqueProcessId;
		DWORD          dHandleCount;
		DWORD          dReserved07;
		DWORD          dReserved08;
		VM_COUNTERS    VmCounters;
		DWORD          dCommitCharge;
		SYSTEM_THREAD  Threads[1];
*/
		} SYSTEM_PROCESS_INFORMATION;

	//////////////////////////////////////////////////////////////////
	// Thread Information: (NtQueryInformationThread)

	typedef enum _THREAD_INFORMATION_CLASS {
		ThreadBasicInformation = 0,
	} THREAD_INFORMATION_CLASS;

	typedef struct _BASIC_THREAD_INFORMATION {
		BOOL		ExitStatus;
		PVOID		Teb;
		CLIENT_ID	ClientID;
		DWORD		AffinityMask;
		DWORD		BasePriority;
		DWORD		Priority;
	} BASIC_THREAD_INFORMATION;

	//////////////////////////////////////////////////////////////////
	// Handle Information: (SystemHandleInformation)

	// NOTE! The following structures are returning 16 bit handles only.
	// 16bit handles are not actual since at least WinXP.

	typedef struct _SYSTEM_HANDLE
	{
		DWORD		ProcessID;
		BYTE		ObjectTypeNumber;
		BYTE		Flags;
		WORD		HandleNumber;
		DWORD_PTR	KernelAddress;
		ACCESS_MASK	GrantedAccess;
	} SYSTEM_HANDLE;

	typedef struct _SYSTEM_HANDLE_INFORMATION
	{
		DWORD			Count;
		SYSTEM_HANDLE	Handles[1];
	} SYSTEM_HANDLE_INFORMATION;

	//////////////////////////////////////////////////////////////////
	// Since Windows XP we can use:

	typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX
	{
		PVOID	Object;
		HANDLE	UniqueProcessId;
		HANDLE	HandleValue;
		ULONG	GrantedAccess;
		USHORT	CreatorBackTraceIndex;
		USHORT	ObjectTypeIndex;
		ULONG	HandleAttributes;
		ULONG	Reserved;

		DWORD GetPid() const
		{
			return (DWORD)UniqueProcessId;
		}

	} SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO_EX;

	typedef struct _SYSTEM_HANDLE_INFORMATION_EX
	{
		ULONG_PTR	NumberOfHandles;
		ULONG_PTR	Reserved;
		SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX Handles[1];
	} SYSTEM_HANDLE_INFORMATION_EX, *PSYSTEM_HANDLE_INFORMATION_EX;

	//////////////////////////////////////////////////////////////////
	// File Information: (NtQueryInformationFile)

	typedef enum _FILE_INFORMATION_CLASS {
		FileNameInformation = 9,
	} FILE_INFORMATION_CLASS;

	typedef struct _FILE_NAME_INFORMATION {
		ULONG	FileNameLength;
		WCHAR	FileName[1];
	} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

	//////////////////////////////////////////////////////////////////
	// Object Information: (NtQueryObject)

	typedef enum _OBJECT_INFORMATION_CLASS {
		ObjectNameInformation = 1,
		ObjectTypeInformation = 2,
	} OBJECT_INFORMATION_CLASS;

	typedef enum _POOL_TYPE {
		NonPagedPool,
		PagedPool,
		NonPagedPoolMustSucceed,
		DontUseThisType,
		NonPagedPoolCacheAligned,
		PagedPoolCacheAligned,
		NonPagedPoolCacheAlignedMustS,
		MaxPoolType
	} POOL_TYPE;


	typedef struct _OBJECT_NAME_INFORMATION {
		UNICODE_STRING			ObjectName;
	} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;


	typedef struct _OBJECT_TYPE_INFORMATION {
		UNICODE_STRING			TypeName;					// 0x0000:
		ULONG					TotalNumberOfHandles;		// 0x0008:
		ULONG					TotalNumberOfObjects;		// 0x000c:
		WCHAR					Unused1[8];					// 0x0010:
		ULONG					HighWaterNumberOfHandles;	// 0x0020:
		ULONG					HighWaterNumberOfObjects;	// 0x0024:
		WCHAR					Unused2[8];					// 0x0028:
		ACCESS_MASK				InvalidAttributes;			// 0x0038:
		GENERIC_MAPPING			GenericMapping;				// 0x003c:
		ACCESS_MASK				ValidAttributes;			// 0x004c:
		BOOLEAN					SecurityRequired;			// 0x0050:
		BOOLEAN					MaintainHandleCount;		// 0x0051:
		USHORT					MaintainTypeList;			// 0x0052:
		POOL_TYPE				PoolType;					// 0x0054:
		ULONG					DefaultPagedPoolCharge;		// 0x0058:
		ULONG					DefaultNonPagedPoolCharge;	// 0x005c:
	} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

	//////////////////////////////////////////////////////////////////
	// Ntdll.dll Functions:

	typedef NTSTATUS (WINAPI *PNtQuerySystemInformation)(
				IN	SYSTEM_INFORMATION_CLASS SystemInformationClass,
				OUT	PVOID				SystemInformation,
				IN	ULONG				SystemInformationLength,
				OUT	PULONG				ReturnLength OPTIONAL );

	typedef NTSTATUS (WINAPI *PNtQueryObject)(
				IN	HANDLE				ObjectHandle OPTIONAL,
				IN	OBJECT_INFORMATION_CLASS ObjectInformationClass,
				OUT	PVOID				ObjectInformation,
				IN	ULONG				Length,
				OUT	PULONG				ResultLength );

	typedef NTSTATUS (WINAPI *PNtQueryInformationThread)(
				IN	HANDLE				ThreadHandle,
				IN	THREAD_INFORMATION_CLASS ThreadInformationClass,
				OUT	PVOID				ThreadInformation,
				IN	ULONG				ThreadInformationLength,
				OUT	PULONG				ReturnLength OPTIONAL );

	typedef NTSTATUS (WINAPI *PNtQueryInformationFile)(
				IN	HANDLE				FileHandle,
				OUT	PIO_STATUS_BLOCK	IoStatusBlock,
				OUT	PVOID				FileInformation,
				IN	ULONG				Length,
				IN	FILE_INFORMATION_CLASS FileInformationClass );

	typedef NTSTATUS (WINAPI *PNtQueryInformationProcess)(
				IN	HANDLE				ProcessHandle,
				IN	PROCESS_INFORMATION_CLASS ProcessInformationClass,
				OUT	PVOID				ProcessInformation,
				IN	ULONG				ProcessInformationLength,
				OUT	PULONG				ReturnLength );


	static PNtQuerySystemInformation	NtQuerySystemInformation;
	static PNtQueryObject				NtQueryObject;
	static PNtQueryInformationThread	NtQueryInformationThread;
	static PNtQueryInformationFile		NtQueryInformationFile;			// not used!
	static PNtQueryInformationProcess	NtQueryInformationProcess;

	static HMODULE						hModule;
	static BOOL							bStatus;

protected:
	static BOOL Init();
};

//////////////////////////////////////////////////////////////////////////////////////
//
// IPsapi
//
//////////////////////////////////////////////////////////////////////////////////////

class IPsapi
{
#if 0
#include <Psapi.h>
#endif

#ifdef PROTECT_INTERFACES
	friend BOOL GetProcessExecutableName( DWORD dwPID, LPTSTR szName, int cbSize );
	protected:
#else
	public:
#endif // ifdef PROTECT_INTERFACES

	typedef struct _PROCESS_MEMORY_COUNTERS {
		DWORD		cb;
		DWORD		PageFaultCount;
		SIZE_T		PeakWorkingSetSize;
		SIZE_T		WorkingSetSize;
		SIZE_T		QuotaPeakPagedPoolUsage;
		SIZE_T		QuotaPagedPoolUsage;
		SIZE_T		QuotaPeakNonPagedPoolUsage;
		SIZE_T		QuotaNonPagedPoolUsage;
		SIZE_T		PagefileUsage;
		SIZE_T		PeakPagefileUsage;
	} PROCESS_MEMORY_COUNTERS;
	typedef PROCESS_MEMORY_COUNTERS *PPROCESS_MEMORY_COUNTERS;

	typedef struct _MODULEINFO {
		LPVOID		lpBaseOfDll;
		DWORD		SizeOfImage;
		LPVOID		EntryPoint;
	} MODULEINFO, *LPMODULEINFO;

	typedef BOOL (WINAPI *PEnumProcesses)(
				DWORD*			lpidProcess,	// array of process identifiers
				DWORD			cb,				// size of array
				DWORD*			cbNeeded		// number of bytes required
			);

	typedef BOOL (WINAPI *PGetProcessMemoryInfo)(
				HANDLE			Process,		// handle to process
				PPROCESS_MEMORY_COUNTERS ppsmemCounters,	// buffer
				DWORD			cb				// size of buffer
			);

	typedef BOOL (WINAPI *PEnumProcessModules)(
				HANDLE			hProcess,		// handle to process
				HMODULE*		lphModule,		// array of module handles
				DWORD			cb,				// size of array
				LPDWORD			lpcbNeeded		// number of bytes required
			);

	typedef DWORD (WINAPI *PGetModuleFileNameEx)(
				HANDLE			hProcess,		// handle to process
				HMODULE			hModule,		// handle to module
				LPTSTR			lpFilename,		// path buffer
				DWORD			nSize			// maximum characters to retrieve
			);

	typedef DWORD (WINAPI *PGetModuleBaseName)(
				HANDLE			hProcess,		// handle to process
				HMODULE			hModule,		// handle to module
				LPTSTR			lpBasename,		// name buffer
				DWORD			nSize			// maximum characters to retrieve
			);

	typedef BOOL (WINAPI *PGetModuleInformation)(
				HANDLE			hProcess,		// handle to process
				HMODULE			hModule,		// handle to module
				LPMODULEINFO	lpmodinfo,		// info buffer
				DWORD			cb				// info buffer size in bytes
			);

	typedef BOOL (WINAPI *PEnumDeviceDrivers)(
				LPVOID*			lpImageBase,	// array of load addresses
				DWORD			cb,				// size of array
				LPDWORD			lpcbNeeded		// number of bytes returned
			);

	typedef DWORD (WINAPI *PGetDeviceDriverBaseName)(
				LPVOID			ImageBase,		// driver load address
				LPTSTR			lpBaseName,		// driver base name buffer
				DWORD			nSize			// size of buffer
			);

	typedef DWORD (WINAPI *PGetDeviceDriverFileName)(
				LPVOID			ImageBase,		// driver load address
				LPTSTR			lpFilename,		// path buffer
				DWORD			nSize			// size of buffer
			);

	typedef DWORD (WINAPI *PGetMappedFileName)(
		HANDLE				hProcess,			// process handle
		LPVOID				lpv,				// base address
		LPTSTR				lpFilename,			// mapped file name
		DWORD				nSize				// size of buffer
	);

	typedef DWORD (WINAPI *PGetProcessImageFileName)(	// Windows XP+ !!!
		HANDLE				hProcess,			// process handle
		LPTSTR				lpImageFileName,	// process executable file name
		DWORD				nSize				// size of buffer
	);


	static PEnumProcesses				EnumProcesses;
	static PGetProcessMemoryInfo		GetProcessMemoryInfo;
	static PEnumProcessModules			EnumProcessModules;
	static PGetModuleFileNameEx			GetModuleFileNameEx;
	static PGetModuleBaseName			GetModuleBaseName;
	static PGetModuleInformation		GetModuleInformation;
	static PEnumDeviceDrivers			EnumDeviceDrivers;
	static PGetDeviceDriverBaseName		GetDeviceDriverBaseName;
	static PGetDeviceDriverFileName		GetDeviceDriverFileName;
	static PGetMappedFileName			GetMappedFileName;
	static PGetProcessImageFileName		GetProcessImageFileName;

	static HMODULE						hModule;
	static BOOL							bStatus;

protected:
	static BOOL Init();
};

//////////////////////////////////////////////////////////////////////////////////////
//
// IToolhelp32
//
//////////////////////////////////////////////////////////////////////////////////////

class IToolhelp32
{
#if 0
#include <Tlhelp32.h>
#endif

public:
	enum{ MAX_MODULE_NAME32 = 255 };

#ifdef PROTECT_INTERFACES
//	friend BOOL GetProcessExecutableName( DWORD dwPID, LPTSTR szName, int cbSize );
	protected:
#else
	public:
#endif // ifdef PROTECT_INTERFACES

	enum
	{
		TH32CS_SNAPHEAPLIST	= 0x00000001,
		TH32CS_SNAPPROCESS	= 0x00000002,
		TH32CS_SNAPTHREAD	= 0x00000004,
		TH32CS_SNAPMODULE	= 0x00000008,
		TH32CS_SNAPALL		= (TH32CS_SNAPHEAPLIST | TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD | TH32CS_SNAPMODULE),
		TH32CS_INHERIT		= 0x80000000
	};

	typedef struct tagPROCESSENTRY32
	{
		DWORD	dwSize;
		DWORD	cntUsage;
		DWORD	th32ProcessID;			// this process
		DWORD_PTR	th32DefaultHeapID;
		DWORD	th32ModuleID;			// associated exe
		DWORD	cntThreads;
		DWORD	th32ParentProcessID;	// this process's parent process
		LONG	pcPriClassBase;			// Base priority of process's threads
		DWORD	dwFlags;
		TCHAR	szExeFile[MAX_PATH];	// Path
	} PROCESSENTRY32;
	typedef PROCESSENTRY32 *  PPROCESSENTRY32;
	typedef PROCESSENTRY32 *  LPPROCESSENTRY32;

	typedef struct tagMODULEENTRY32
	{
		DWORD	dwSize;
		DWORD	th32ModuleID;			// This module
		DWORD	th32ProcessID;			// owning process
		DWORD	GlblcntUsage;			// Global usage count on the module
		DWORD	ProccntUsage;			// Module usage count in th32ProcessID's context
		BYTE*	modBaseAddr;			// Base address of module in th32ProcessID's context
		DWORD	modBaseSize;			// Size in bytes of module starting at modBaseAddr
		HMODULE	hModule;				// The hModule of this module in th32ProcessID's context
		TCHAR	szModule[MAX_MODULE_NAME32 + 1];
		TCHAR	szExePath[MAX_PATH];
	} MODULEENTRY32;
	typedef MODULEENTRY32 *  PMODULEENTRY32;
	typedef MODULEENTRY32 *  LPMODULEENTRY32;

	typedef struct tagTHREADENTRY32
	{
		DWORD	dwSize;
		DWORD	cntUsage;
		DWORD	th32ThreadID;			// this thread
		DWORD	th32OwnerProcessID;		// Process this thread is associated with
		LONG	tpBasePri;
		LONG	tpDeltaPri;
		DWORD	dwFlags;
	} THREADENTRY32;
	typedef THREADENTRY32 *  PTHREADENTRY32;
	typedef THREADENTRY32 *  LPTHREADENTRY32;

	typedef struct tagHEAPLIST32
	{
		SIZE_T		dwSize;
		DWORD		th32ProcessID;		// owning process
		DWORD_PTR	th32HeapID;			// heap (in owning process's context!)
		DWORD		dwFlags;
	} HEAPLIST32;
	typedef HEAPLIST32 *  PHEAPLIST32;
	typedef HEAPLIST32 *  LPHEAPLIST32;
	//
	// dwFlags
	//
	enum
	{
		HF32_DEFAULT	= 1,	// process's default heap
		HF32_SHARED		= 2		// is shared heap
	};

	typedef struct tagHEAPENTRY32
	{
		DWORD_PTR	dwSize;
		HANDLE		hHandle;			// Handle of this heap block
		DWORD_PTR	dwAddress;			// Linear address of start of block
		DWORD_PTR	dwBlockSize;		// Size of block in bytes
		DWORD	dwFlags;
		DWORD	dwLockCount;
		DWORD	dwResvd;
		DWORD	th32ProcessID;			// owning process
		DWORD	th32HeapID;				// heap block is in
	} HEAPENTRY32;
	typedef HEAPENTRY32 *  PHEAPENTRY32;
	typedef HEAPENTRY32 *  LPHEAPENTRY32;
	//
	// dwFlags
	//
	enum
	{
		LF32_FIXED		= 0x00000001,
		LF32_FREE		= 0x00000002,
		LF32_MOVEABLE	= 0x00000004
	};

	typedef HANDLE (WINAPI *PCreateToolhelp32Snapshot)(
		DWORD				dwFlags,
		DWORD				th32ProcessID
		);

	typedef BOOL (WINAPI *PProcess32First)(
		HANDLE				hSnapshot,
		LPPROCESSENTRY32	lppe
		);

	typedef BOOL (WINAPI *PProcess32Next)(
		HANDLE				hSnapshot,
		LPPROCESSENTRY32	lppe
		);

	typedef BOOL (WINAPI *PModule32First)(
		HANDLE				hSnapshot,
		LPMODULEENTRY32		lpme
		);

	typedef BOOL (WINAPI *PModule32Next)(
		HANDLE				hSnapshot,
		LPMODULEENTRY32		lpme
		);

	typedef BOOL (WINAPI *PThread32First)(
		HANDLE				hSnapshot,
		LPTHREADENTRY32		lpte
		);

	typedef BOOL (WINAPI *PThread32Next)(
		HANDLE				hSnapshot,
		LPTHREADENTRY32		lpte
		);

	typedef BOOL (WINAPI *PHeap32ListFirst)(
		HANDLE				hSnapshot,
		LPHEAPLIST32		lphl
		);

	typedef BOOL (WINAPI *PHeap32ListNext)(
		HANDLE				hSnapshot,
		LPHEAPLIST32		lphl
		);

	// Note: The heap block functions do not reference a snapshot and
	// just walk the process's heap from the beginning each time. Infinite 
	// loops can occur if the target process changes its heap while the
	// functions below are enumerating the blocks in the heap.

	typedef BOOL (WINAPI *PHeap32First)(
		LPHEAPENTRY32		phe,
		DWORD				dwProcessID,
		UINT_PTR			dwHeapID
		);

	typedef BOOL (WINAPI *PHeap32Next)(
		LPHEAPENTRY32		phe
		);


	static PCreateToolhelp32Snapshot	CreateToolhelp32Snapshot;

	static PProcess32First				Process32First;
	static PProcess32Next				Process32Next;

	static PModule32First				Module32First;
	static PModule32Next				Module32Next;

	static PThread32First				Thread32First;
	static PThread32Next				Thread32Next;

	static PHeap32ListFirst				Heap32ListFirst;
	static PHeap32ListNext				Heap32ListNext;

	static PHeap32First					Heap32First;
	static PHeap32Next					Heap32Next;

	static HMODULE						hModule;
	static BOOL							bStatus;

protected:
	static BOOL Init();
};

//////////////////////////////////////////////////////////////////////////////////////
//
// IAdvapi32
//
//////////////////////////////////////////////////////////////////////////////////////

class IAdvapi32
{
#if 0
#include <Winsvc.h>
#endif

#ifdef PROTECT_INTERFACES
//	friend BOOL GetProcessExecutableName( DWORD dwPID, LPTSTR szName, int cbSize );
	protected:
#else
	public:
#endif // ifdef PROTECT_INTERFACES

	//
	// Info levels for EnumServicesStatusEx
	//
	typedef enum _SC_ENUM_TYPE {
			SC_ENUM_PROCESS_INFO        = 0
	} SC_ENUM_TYPE;

	typedef struct _SERVICE_STATUS_PROCESS {
		DWORD	dwServiceType;
		DWORD	dwCurrentState;
		DWORD	dwControlsAccepted;
		DWORD	dwWin32ExitCode;
		DWORD	dwServiceSpecificExitCode;
		DWORD	dwCheckPoint;
		DWORD	dwWaitHint;
		DWORD	dwProcessId;
		DWORD	dwServiceFlags;
	} SERVICE_STATUS_PROCESS, *LPSERVICE_STATUS_PROCESS;

	//
	// Service Status Enumeration Structure
	//

	typedef struct _ENUM_SERVICE_STATUS_PROCESS {
		LPTSTR						lpServiceName;
		LPTSTR						lpDisplayName;
		SERVICE_STATUS_PROCESS		ServiceStatusProcess;
	} ENUM_SERVICE_STATUS_PROCESS, *LPENUM_SERVICE_STATUS_PROCESS;


	typedef BOOL (WINAPI *PEnumServicesStatusEx)(
		SC_HANDLE						hSCManager,
		SC_ENUM_TYPE					InfoLevel,
		DWORD							dwServiceType,
		DWORD							dwServiceState,
		LPBYTE							lpServices,
		DWORD							cbBufSize,
		LPDWORD							pcbBytesNeeded,
		LPDWORD							lpServicesReturned,
		LPDWORD							lpResumeHandle,
		LPCTSTR							pszGroupName
		);

	static PEnumServicesStatusEx		EnumServicesStatusEx;

	static HMODULE						hModule;
	static BOOL							bStatus;

protected:
	static BOOL Init();
};

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////


#endif //ifndef _WINDOWSCORE_H_UID0000002C4BF67042
