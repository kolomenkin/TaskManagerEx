//////////////////////////////////////////////////////////////
// File:		// WindowsCore.cpp
// File time:	// 30.03.2005	1:09
// Version: 	// 1.0
// Author:		// Sergey Kolomenkin <register@tut.by>
// Copyright:	// (c) Sergey Kolomenkin, Minsk, 2005
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#include "stdafx.h"
//#include <windows.h>
#include "WindowsCore.h"

//////////////////////////////////////////////////////////////////////////////////////
//
// INtDll
//
//////////////////////////////////////////////////////////////////////////////////////
INtDll::PNtQuerySystemInformation		INtDll::NtQuerySystemInformation	= NULL;
INtDll::PNtQueryObject					INtDll::NtQueryObject				= NULL;
INtDll::PNtQueryInformationThread		INtDll::NtQueryInformationThread	= NULL;
INtDll::PNtQueryInformationFile			INtDll::NtQueryInformationFile		= NULL;
INtDll::PNtQueryInformationProcess		INtDll::NtQueryInformationProcess	= NULL;

HMODULE	INtDll::hModule = NULL;
BOOL	INtDll::bStatus = INtDll::Init();

BOOL INtDll::Init()
{
	hModule = LoadLibrary( _T("ntdll.dll") );
	//TRACE( _T("TaskManagerEx: INtDll::Init(): hModule = 0x%08X\n"), hModule );

	NtQuerySystemInformation = (PNtQuerySystemInformation)
					GetProcAddress( hModule, "NtQuerySystemInformation" );

	NtQueryObject = (PNtQueryObject)
					GetProcAddress(	hModule, "NtQueryObject" );

	NtQueryInformationThread = (PNtQueryInformationThread)
					GetProcAddress(	hModule, "NtQueryInformationThread" );

	NtQueryInformationFile = (PNtQueryInformationFile)
					GetProcAddress(	hModule, "NtQueryInformationFile" );

	NtQueryInformationProcess = (PNtQueryInformationProcess)
					GetProcAddress( hModule, "NtQueryInformationProcess" );

	return  NtQuerySystemInformation	!= NULL &&
			NtQueryObject				!= NULL &&
			NtQueryInformationThread	!= NULL &&
			NtQueryInformationFile		!= NULL &&
			NtQueryInformationProcess	!= NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// IPsapi
//
//////////////////////////////////////////////////////////////////////////////////////
IPsapi::PEnumProcesses					IPsapi::EnumProcesses				= NULL;
IPsapi::PGetProcessMemoryInfo			IPsapi::GetProcessMemoryInfo		= NULL;
IPsapi::PEnumProcessModules				IPsapi::EnumProcessModules			= NULL;
IPsapi::PGetModuleFileNameEx			IPsapi::GetModuleFileNameEx			= NULL;
IPsapi::PGetModuleBaseName				IPsapi::GetModuleBaseName			= NULL;
IPsapi::PGetModuleInformation			IPsapi::GetModuleInformation		= NULL;
IPsapi::PEnumDeviceDrivers				IPsapi::EnumDeviceDrivers			= NULL;
IPsapi::PGetDeviceDriverBaseName		IPsapi::GetDeviceDriverBaseName		= NULL;
IPsapi::PGetDeviceDriverFileName		IPsapi::GetDeviceDriverFileName		= NULL;
IPsapi::PGetMappedFileName				IPsapi::GetMappedFileName			= NULL;
IPsapi::PGetProcessImageFileName		IPsapi::GetProcessImageFileName		= NULL;	// Windows XP+

HMODULE	IPsapi::hModule = NULL;
BOOL	IPsapi::bStatus = IPsapi::Init();

BOOL IPsapi::Init()
{
	hModule = LoadLibrary( _T("psapi.dll") );
	//TRACE( _T("TaskManagerEx: IPsapi::Init(): hModule = 0x%08X\n"), hModule );

	EnumProcesses = (PEnumProcesses)
					GetProcAddress( hModule, "EnumProcesses" );

	GetProcessMemoryInfo = (PGetProcessMemoryInfo)
					GetProcAddress(	hModule, "GetProcessMemoryInfo" );

	EnumProcessModules = (PEnumProcessModules)
					GetProcAddress( hModule, "EnumProcessModules" );

	GetModuleFileNameEx = (PGetModuleFileNameEx)
					GetProcAddress(	hModule, "GetModuleFileNameEx" FUNC_SUFFIX );

	GetModuleBaseName = (PGetModuleBaseName)
					GetProcAddress(	hModule, "GetModuleBaseName" FUNC_SUFFIX );

	GetModuleInformation = (PGetModuleInformation)
					GetProcAddress(	hModule, "GetModuleInformation" );

	EnumDeviceDrivers = (PEnumDeviceDrivers)
					GetProcAddress(	hModule, "EnumDeviceDrivers" );

	GetDeviceDriverBaseName = (PGetDeviceDriverBaseName)
					GetProcAddress(	hModule, "GetDeviceDriverBaseName" FUNC_SUFFIX );

	GetDeviceDriverFileName = (PGetDeviceDriverFileName)
					GetProcAddress(	hModule, "GetDeviceDriverFileName" FUNC_SUFFIX );

	GetMappedFileName = (PGetMappedFileName)
					GetProcAddress(	hModule, "GetMappedFileName" FUNC_SUFFIX );

	GetProcessImageFileName = (PGetProcessImageFileName)
					GetProcAddress(	hModule, "GetProcessImageFileName" FUNC_SUFFIX );	// Windows XP+


	return  EnumProcesses				!= NULL &&
			GetProcessMemoryInfo		!= NULL &&
			EnumProcessModules			!= NULL &&
			GetModuleFileNameEx			!= NULL &&
			GetModuleBaseName			!= NULL &&
			GetModuleInformation		!= NULL &&
			EnumDeviceDrivers			!= NULL &&
			GetDeviceDriverBaseName		!= NULL &&
			GetDeviceDriverFileName		!= NULL &&
			GetMappedFileName			!= NULL &&
//			GetProcessImageFileName		!= NULL &&	// Windows XP+
			TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// IToolhelp32
//
//////////////////////////////////////////////////////////////////////////////////////
IToolhelp32::PCreateToolhelp32Snapshot	IToolhelp32::CreateToolhelp32Snapshot	= NULL;
IToolhelp32::PProcess32First			IToolhelp32::Process32First				= NULL;
IToolhelp32::PProcess32Next				IToolhelp32::Process32Next				= NULL;
IToolhelp32::PModule32First				IToolhelp32::Module32First				= NULL;
IToolhelp32::PModule32Next				IToolhelp32::Module32Next				= NULL;
IToolhelp32::PThread32First				IToolhelp32::Thread32First				= NULL;
IToolhelp32::PThread32Next				IToolhelp32::Thread32Next				= NULL;
IToolhelp32::PHeap32ListFirst			IToolhelp32::Heap32ListFirst			= NULL;
IToolhelp32::PHeap32ListNext			IToolhelp32::Heap32ListNext				= NULL;
IToolhelp32::PHeap32First				IToolhelp32::Heap32First				= NULL;
IToolhelp32::PHeap32Next				IToolhelp32::Heap32Next					= NULL;

HMODULE	IToolhelp32::hModule = NULL;
BOOL	IToolhelp32::bStatus = IToolhelp32::Init();

BOOL IToolhelp32::Init()
{
	hModule = LoadLibrary( _T("kernel32.dll") );
	//TRACE( _T("TaskManagerEx: IToolhelp32::Init(): hModule = 0x%08X\n"), hModule );

	CreateToolhelp32Snapshot = (PCreateToolhelp32Snapshot)
					GetProcAddress( hModule, "CreateToolhelp32Snapshot" );

	Process32First = (PProcess32First)
					GetProcAddress(	hModule, "Process32First" FUNC_SUFFIX_ALT );

	Process32Next = (PProcess32Next)
					GetProcAddress(	hModule, "Process32Next" FUNC_SUFFIX_ALT );

	Module32First = (PModule32First)
					GetProcAddress(	hModule, "Module32First" FUNC_SUFFIX_ALT );

	Module32Next = (PModule32Next)
					GetProcAddress(	hModule, "Module32Next" FUNC_SUFFIX_ALT );

	Thread32First = (PThread32First)
					GetProcAddress(	hModule, "Thread32First" );

	Thread32Next = (PThread32Next)
					GetProcAddress(	hModule, "Thread32Next" );

	Heap32ListFirst = (PHeap32ListFirst)
					GetProcAddress(	hModule, "Heap32ListFirst" );

	Heap32ListNext = (PHeap32ListNext)
					GetProcAddress(	hModule, "Heap32ListNext" );

	Heap32First = (PHeap32First)
					GetProcAddress(	hModule, "Heap32First" );

	Heap32Next = (PHeap32Next)
					GetProcAddress(	hModule, "Heap32Next" );


	return  CreateToolhelp32Snapshot	!= NULL &&
			Module32First				!= NULL &&
			Module32Next				!= NULL &&
			Process32First				!= NULL &&
			Process32Next				!= NULL &&
			Thread32First				!= NULL &&
			Thread32Next				!= NULL &&
			Heap32ListFirst				!= NULL &&
			Heap32ListNext				!= NULL &&
			Heap32First					!= NULL &&
			Heap32Next					!= NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// IPsapi
//
//////////////////////////////////////////////////////////////////////////////////////
IAdvapi32::PEnumServicesStatusEx		IAdvapi32::EnumServicesStatusEx		= NULL; // Windows 2000+

HMODULE	IAdvapi32::hModule = NULL;
BOOL	IAdvapi32::bStatus = IAdvapi32::Init();

BOOL IAdvapi32::Init()
{
	hModule = LoadLibrary( _T("advapi32.dll") );
	//TRACE( _T("TaskManagerEx: IAdvapi32::Init(): hModule = 0x%08X\n"), hModule );

	EnumServicesStatusEx = (PEnumServicesStatusEx)
					GetProcAddress( hModule, "EnumServicesStatusEx" FUNC_SUFFIX );

	return  EnumServicesStatusEx		!= NULL &&
			TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
