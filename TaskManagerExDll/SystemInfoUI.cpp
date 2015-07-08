//////////////////////////////////////////////////////////////
// File:		// SystemInfoUI.cpp
// File time:	// 31.03.2005	22:26
// Version: 	// 1.0
// Author:		// Sergey Kolomenkin <register@tut.by>
// Copyright:	// (c) Sergey Kolomenkin, Minsk, 2005
// Description: //////////////////////////////////////////////
//   Some useful routines...
// 

#include "stdafx.h"
//#include <windows.h>
#include "SystemInfoUI.h"
#include "DllVersion.h"
#include "TaskManagerExDll.h"
#include "resource.h"
#include "Localization.h"

//////////////////////////////////////////////////////////////
//
// SystemModuleInformation::MODULE_INFO
//
//////////////////////////////////////////////////////////////

void SystemModuleInformation::MODULE_INFO::InsertColumns( CSystemInfoListCtrl& list, BOOL bPid )
{
	int sub = 0;

	list.InsertColumn( sub+0, COLUMN_MODULE_BASE );
	list.InsertColumn( sub+1, COLUMN_MODULE_LENGTH, LVCFMT_RIGHT );

	if( bPid )
	{
		list.InsertColumn( sub+2, COLUMN_PID, LVCFMT_RIGHT );
		sub++;
		list.InsertColumn( sub+2, COLUMN_PROCESS_NAME );
		sub++;
	}

	list.InsertColumn( sub+2, COLUMN_MODULE_USAGE, LVCFMT_RIGHT );
	list.InsertColumn( sub+3, COLUMN_MODULE_NAME );
	list.InsertColumn( sub+4, COLUMN_MODULE_FILE_SIZE, LVCFMT_RIGHT );
	list.InsertColumn( sub+5, COLUMN_MODULE_VERSION );
	list.InsertColumn( sub+6, COLUMN_MODULE_PATH );

	if( bPid )
	{
		list.SetSortedColumn( 2 ); //COLUMN_PID;
	}
	else
	{
		list.SetSortedColumn( 0 ); //COLUMN_MODULE_BASE;
	}
	list.SetSortAscending( TRUE );
}

int SystemModuleInformation::MODULE_INFO::Insert( CSystemInfoListCtrl& list, BOOL bPid,
	size_t iItem, size_t iItemCount) const
{
	iItem;	// use var
	iItemCount;	// use var

	CString strPID, strProcesName;
	CString strBase, strLength, strUsage, strFileSize, strVersion, strFilePath;

	// Get size
	INT64 i64FileSize = 0;
	LPCTSTR szFileName = _tcslen(FileFullPath) > 0 ? FileFullPath : FullPath;
	CFile file;
	if ( file.Open( szFileName, CFile::modeRead ) )
	{
		i64FileSize = file.GetLength();
		file.Close();
	}

	strBase.Format( _T("0x%08IX"), Handle );
	strLength.Format( _T("%d"), info.SizeOfImage );

	if( me32.GlblcntUsage == 0 )
		strUsage.Format( _T("") );
	else if( me32.GlblcntUsage == 65535 )
		strUsage = LocLoadString(IDS_MODULE_FIXED);
	else
		strUsage.Format( _T("%d"), me32.GlblcntUsage );

	strFileSize.Format( _T("%I64d"), i64FileSize );
	GetModuleVersion( szFileName, strVersion );

	strFilePath = FullPath;
	if( _tcsicmp( FullPath, szFileName ) != 0 )
	{
		strFilePath += _T("    ==>    ");
		strFilePath += FileFullPath;
	}

	int sub = 0;

	int iListItemCount = list.GetItemCount();
	int nPos = list.InsertItem( iListItemCount, _T("") );

	list.SetItemText( nPos, sub+0, strBase );
	list.SetItemText( nPos, sub+1, strLength );

	if( bPid )
	{
		strPID.Format( _T("%d"), ProcessId );
		list.SetItemText( nPos, sub+2, strPID );
		sub++;

		strProcesName = GetProcessName( ProcessId );
		list.SetItemText( nPos, sub+2, strProcesName );
		sub++;
	}

	list.SetItemText( nPos, sub+2, strUsage );
	list.SetItemText( nPos, sub+3, Name );
	list.SetItemText( nPos, sub+4, strFileSize );
	list.SetItemText( nPos, sub+5, strVersion );
	list.SetItemText( nPos, sub+6, strFilePath );

	return nPos;
}

//////////////////////////////////////////////////////////////
//
// SystemKernelModuleInformation::KERNEL_MODULE_INFORMATION
//
//////////////////////////////////////////////////////////////

void SystemKernelModuleInformation::KERNEL_MODULE_INFORMATION::InsertColumns( CSystemInfoListCtrl& list )
{
	list.InsertColumn( 0, COLUMN_DRIVER_NAME );
	list.InsertColumn( 1, COLUMN_DRIVER_ADDRESS );
	list.InsertColumn( 2, COLUMN_DRIVER_TYPE );
	list.InsertColumn( 3, COLUMN_DRIVER_PATH );
	list.InsertColumn( 4, COLUMN_DRIVER_FILE_SIZE, LVCFMT_RIGHT );
	list.InsertColumn( 5, COLUMN_DRIVER_FULL_NAME );

	list.SetSortedColumn( 0 ); //COLUMN_DRIVER_NAME;
//	list.SetSortedColumn( 1 ); //COLUMN_DRIVER_ADDRESS;
	list.SetSortAscending( TRUE );
}

int SystemKernelModuleInformation::KERNEL_MODULE_INFORMATION::Insert( CSystemInfoListCtrl& list,
	size_t iItem, size_t iItemCount) const
{
	iItem;	// use var
	iItemCount;	// use var

	CString strBaseAddress, strBaseName, strFullPath, strExtension, strFileSize, strFullFileName;

	strBaseAddress.Format( _T("0x%08IX"), pBaseAddress );
	strBaseName = Name;
	strFullPath = FullPath;
	int pos = strBaseName.ReverseFind( _T('.') );
	if( pos != -1 )
	{
		strExtension = strBaseName.Mid( pos + 1 );
		strExtension.MakeLower();
	}
	strBaseName.MakeLower();
	strFullFileName = SystemInfoUtils::DecodeModuleName( strFullPath );

	// Get size
	INT64 i64FileSize = 0;
	CFile file;
	if ( file.Open( strFullFileName, CFile::modeRead ) )
	{
		i64FileSize = file.GetLength();
		strFileSize.Format( _T("%I64d"), i64FileSize );
		file.Close();
	}

	int iListItemCount = list.GetItemCount();
	int nPos = list.InsertItem( iListItemCount, _T("") );

	list.SetItemText( nPos, 0, strBaseName );
	list.SetItemText( nPos, 1, strBaseAddress );
	list.SetItemText( nPos, 2, strExtension );
	list.SetItemText( nPos, 3, strFullPath );
	list.SetItemText( nPos, 4, strFileSize );
	list.SetItemText( nPos, 5, strFullFileName );

	return nPos;
}

//////////////////////////////////////////////////////////////
//
// SystemMemoryMapInformation::MEMORY_INFORMATION
//
//////////////////////////////////////////////////////////////

void SystemMemoryMapInformation::MEMORY_INFORMATION::InsertColumns( CSystemInfoListCtrl& list )
{
	list.InsertColumn( 0, COLUMN_MEMORY_ADDRESS );
	list.InsertColumn( 1, COLUMN_MEMORY_SIZE, LVCFMT_RIGHT );
	list.InsertColumn( 2, COLUMN_MEMORY_TYPE );
	list.InsertColumn( 3, COLUMN_MEMORY_BLOCKS, LVCFMT_RIGHT );
	list.InsertColumn( 4, COLUMN_MEMORY_PROTECT );
	list.InsertColumn( 5, COLUMN_MEMORY_DESCRIPTION );

	list.SetSortedColumn( 0 ); //COLUMN_MEMORY_ADDRESS;
	list.SetSortAscending( TRUE );
}

CString GetMemStorageText(DWORD dwStorage)
{
	CString p = COLUMN_MEMORY_TYPE_UNKNOWN;
	switch (dwStorage)
	{
	case MEM_FREE:		p = COLUMN_MEMORY_TYPE_FREE; break;
	case MEM_RESERVE:	p = COLUMN_MEMORY_TYPE_RESERVE; break;
	case MEM_IMAGE:		p = COLUMN_MEMORY_TYPE_IMAGE; break;
	case MEM_MAPPED:	p = COLUMN_MEMORY_TYPE_MAPPED; break;
	case MEM_PRIVATE:	p = COLUMN_MEMORY_TYPE_PRIVATE; break;
	}
	return(p);
}

CString GetProtectText(DWORD dwProtect, BOOL fShowFlags)
{
	CString p = TEXT("Unknown");
	switch (dwProtect & 0xFF) // ~(PAGE_GUARD | PAGE_NOCACHE | PAGE_WRITECOMBINE)
	{
	case PAGE_NOACCESS:				p = TEXT("----"); break;
	case PAGE_READONLY:				p = TEXT("-R--"); break;
	case PAGE_READWRITE:			p = TEXT("-RW-"); break;
	case PAGE_WRITECOPY:			p = TEXT("-RWC"); break;
	case PAGE_EXECUTE:				p = TEXT("E---"); break;
	case PAGE_EXECUTE_READ:			p = TEXT("ER--"); break;
	case PAGE_EXECUTE_READWRITE:	p = TEXT("ERW-"); break;
	case PAGE_EXECUTE_WRITECOPY:	p = TEXT("ERWC"); break;
	}

	if (fShowFlags)
	{
		p += TEXT(" ");
		p += (dwProtect & PAGE_GUARD)			? TEXT("G") : TEXT("-");
		p += (dwProtect & PAGE_NOCACHE)			? TEXT("N") : TEXT("-");
		p += (dwProtect & PAGE_WRITECOMBINE)	? TEXT("W") : TEXT("-");
	}
	return p;
}

void AppendString( CString& s, LPCTSTR szAdd )
{
	int len = (int)_tcslen( szAdd );
	if( len == 0 )
	{
		return;
	}

	if( !s.IsEmpty() )
	{
		s += _T("    ==>    ");
	}
	s += szAdd;
}

int SystemMemoryMapInformation::MEMORY_INFORMATION::Insert( CSystemInfoListCtrl& list,
	size_t iItem, size_t iItemCount, BOOL bExpandRegions) const
{
	iItem;	// use var
	iItemCount;	// use var

	CString strBaseAddress, strSize, strType, strBlockCount, strProtect, strDescription;

	strBaseAddress.Format( _T("0x%08IX"),
		(bRegion ? vmq.pvRgnBaseAddress : vmq.pvBlkBaseAddress ) );
	strSize.Format( _T("%Id"),
		(bRegion ? vmq.RgnSize : vmq.BlkSize ) );
	strType = GetMemStorageText( (bRegion ? vmq.dwRgnStorage : vmq.dwBlkStorage ) );

	if( vmq.dwBlkStorage != MEM_FREE )
	{
		strProtect = GetProtectText( 
			(bRegion ? vmq.dwRgnProtection : vmq.dwBlkProtection ), FALSE );
	}

	if( bRegion )
	{
		if( vmq.dwBlkStorage != MEM_FREE )
		{
			strBlockCount.Format( _T("%d"), vmq.dwRgnBlocks );
		}

		if( vmq.fRgnIsAStack )
		{
			AppendString( strDescription, LocLoadString(IDS_MEMORY_THREAD_STACK) );
		}
		AppendString( strDescription, Module );
		AppendString( strDescription, MappedFile );
	}

	int iListItemCount = list.GetItemCount();
	int nPos = list.InsertItem( iListItemCount, _T("") );

	if( bExpandRegions && !bRegion )
	{
		LVITEM item;
		ZeroMemory( &item, sizeof(item) );
		item.iItem = nPos;
		item.iSubItem = 0; // whole item
		item.mask = LVIF_INDENT;
		item.iIndent = 20 / LIST_IMAGE_WIDTH;
		list.SetItem( &item );
	}

	list.SetItemText( nPos, 0, strBaseAddress );
	list.SetItemText( nPos, 1, strSize );
	list.SetItemText( nPos, 2, strType );
	list.SetItemText( nPos, 3, strBlockCount );
	list.SetItemText( nPos, 4, strProtect );
	list.SetItemText( nPos, 5, strDescription );

	return nPos;
}

//////////////////////////////////////////////////////////////
//
// SystemThreadInformation::THREAD_INFORMATION
//
//////////////////////////////////////////////////////////////

void SystemThreadInformation::THREAD_INFORMATION::InsertColumns( CSystemInfoListCtrl& list, BOOL bPid )
{
	bPid = bPid;
	ASSERT( bPid == FALSE && "Not Implemented!" );

	list.InsertColumn( 0, COLUMN_HANDLE );
	list.InsertColumn( 1, COLUMN_HANDLE_PID, LVCFMT_RIGHT );
	list.InsertColumn( 2, COLUMN_THREAD_TID );
	list.InsertColumn( 3, COLUMN_THREAD_PRIORITY );
	list.InsertColumn( 4, COLUMN_THREAD_START_ADDRESS );
	list.InsertColumn( 5, COLUMN_THREAD_MODULE );
	list.InsertColumn( 6, COLUMN_THREAD_STATE, LVCFMT_RIGHT );
	list.InsertColumn( 7, COLUMN_THREAD_WAIT_REASON, LVCFMT_RIGHT );
	list.InsertColumn( 8, COLUMN_THREAD_CONTEXT_SWITCHES, LVCFMT_RIGHT );
	list.InsertColumn( 9, COLUMN_THREAD_WAIT_TIME, LVCFMT_RIGHT );
}

int SystemThreadInformation::THREAD_INFORMATION::Insert( CSystemInfoListCtrl& list, BOOL bPid,
	size_t iItem, size_t iItemCount) const
{
	iItem;	// use var
	iItemCount;	// use var

	bPid = bPid;
	ASSERT( bPid == FALSE && "Not Implemented!" );

	CString strHandle, strHandlePid, strThreadId, strPriority, strStartAddress, strModule;
	CString strState, strWaitReason, strContextSwitches, strWaitTime;

	if( ThreadHandle != NULL )
	{
		strHandle.Format( _T("0x%08IX"), ThreadHandle );
		strHandlePid.Format( _T("%d"), HandleProcessId );
	}
	else
	{
		strHandle.Empty();
		strHandlePid.Empty();
	}

	strThreadId.Format( _T("0x%08IX"), sti.ClientId.UniqueThread );
	strPriority.Format( _T("%d / %d"), sti.Priority, sti.BasePriority );
	strStartAddress.Format( _T("0x%08IX"), sti.StartAddress );
	strModule = Module;
	int pos = strModule.ReverseFind( _T('\\') );
	strModule = strModule.Mid( pos+1 );
	strState.Format( _T("%d"), sti.State );
	strWaitReason.Format( _T("%d"), sti.WaitReason );
	strContextSwitches.Format( _T("%d"), sti.ContextSwitchCount );
	strWaitTime.Format( _T("%d"), sti.WaitTime );

	int iListItemCount = list.GetItemCount();
	int nPos = list.InsertItem( iListItemCount, _T("") );

	list.SetItemText( nPos, 0, strHandle );
	list.SetItemText( nPos, 1, strHandlePid );
	list.SetItemText( nPos, 2, strThreadId );
	list.SetItemText( nPos, 3, strPriority );
	list.SetItemText( nPos, 4, strStartAddress );
	list.SetItemText( nPos, 5, strModule );
	list.SetItemText( nPos, 6, strState );
	list.SetItemText( nPos, 7, strWaitReason );
	list.SetItemText( nPos, 8, strContextSwitches );
	list.SetItemText( nPos, 9, strWaitTime );

	return nPos;
}

//////////////////////////////////////////////////////////////
//
// SystemHandleInformation::HANDLE_INFORMATION
//
//////////////////////////////////////////////////////////////

void SystemHandleInformation::HANDLE_INFORMATION::InsertColumns( CSystemInfoListCtrl& list, BOOL bPid )
{
	int sub = 0;
	list.InsertColumn(  sub+0, COLUMN_HANDLE );

	if( bPid )
	{
		list.InsertColumn( sub+1, COLUMN_PID, LVCFMT_RIGHT );
		sub++;
		list.InsertColumn( sub+1, COLUMN_PROCESS_NAME );
		sub++;
	}

	list.InsertColumn(  sub+1, COLUMN_HANDLE_ACCESS );
	list.InsertColumn(  sub+2, COLUMN_HANDLE_TYPE );
	list.InsertColumn(  sub+3, COLUMN_HANDLE_TYPE_NUMBER, LVCFMT_RIGHT );
	list.InsertColumn(  sub+4, COLUMN_HANDLE_NAME );

	if( bPid )
	{
		list.SetSortedColumn( 1 ); //COLUMN_PID;
	}
	else
	{
		list.SetSortedColumn( 0 ); //COLUMN_HANDLE;
	}
	list.SetSortAscending( TRUE );
}

int SystemHandleInformation::HANDLE_INFORMATION::Insert( CSystemInfoListCtrl& list, BOOL bPid,
	size_t iItem, size_t iItemCount) const
{
	iItem;	// use var
	iItemCount;	// use var

	CString strPID, strProcesName;
	CString strHandle, strAccess, strType, strTypeNum, strName;

	strHandle.Format( _T("0x%08X"), sh.HandleNumber );
	strAccess.Format( _T("0x%08X"), sh.Flags );

	SystemHandleInformation::GetTypeToken( (HANDLE)sh.HandleNumber, strType, sh.ProcessID );
	SystemHandleInformation::GetName( (HANDLE)sh.HandleNumber, strName, sh.ProcessID );

	const DWORD typeMask = 0xFF;
	DWORD typeFlags = sh.HandleType & (~typeMask);	// In fact I don't know what it means in Windows internals
	strTypeNum.Format( _T("%d"), (sh.HandleType & typeMask) );
	if( typeFlags != 0 )
	{
		CString d;
		d.Format( _T("0x%X + "), typeFlags );
		strTypeNum = d + strTypeNum;
	}

	int sub = 0;

	int iListItemCount = list.GetItemCount();
	int nPos = list.InsertItem( iListItemCount, _T("") );

	list.SetItemText( nPos, sub+0, strHandle );

	if( bPid )
	{
		strPID.Format( _T("%d"), sh.ProcessID );
		list.SetItemText( nPos, sub+1, strPID );
		sub++;

		strProcesName = GetProcessName( sh.ProcessID );
		list.SetItemText( nPos, sub+1, strProcesName );
		sub++;
	}

	list.SetItemText( nPos, sub+1, strAccess );
	list.SetItemText( nPos, sub+2, strType );
	list.SetItemText( nPos, sub+3, strTypeNum );
	list.SetItemText( nPos, sub+4, strName );

	return nPos;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//// continue SystemHandleInformation::HANDLE_INFORMATION

void SystemHandleInformation::HANDLE_INFORMATION::InsertFileColumns( CSystemInfoListCtrl& list, BOOL bPid )
{
	int sub = 0;

	list.InsertColumn(  sub+0, COLUMN_HANDLE );

	if( bPid )
	{
		list.InsertColumn( sub+1, COLUMN_PID, LVCFMT_RIGHT );
		sub++;
		list.InsertColumn( sub+1, COLUMN_PROCESS_NAME );
		sub++;
	}

	list.InsertColumn(  sub+1, COLUMN_FILE_NAME );
	list.InsertColumn(  sub+2, COLUMN_FILE_PATH );
	list.InsertColumn(  sub+3, COLUMN_FILE_DEVICE );

	if( bPid )
	{
		list.SetSortedColumn( 1 ); //COLUMN_PID;
	}
	else
	{
		list.SetSortedColumn( 0 ); //COLUMN_HANDLE;
	}
	list.SetSortAscending( TRUE );
}

int SystemHandleInformation::HANDLE_INFORMATION::InsertFile( CSystemInfoListCtrl& list, BOOL bPid,
	size_t iItem, size_t iItemCount, LPCTSTR szDevice, LPCTSTR szPath) const
{
	iItem;	// use var
	iItemCount;	// use var

	CString strPID, strProcesName;
	CString strHandle, strName, strPath, strDevice;

	strHandle.Format( _T("0x%08X"), sh.HandleNumber );

	strPath = szPath;
	strDevice = szDevice;

	int pos = strDevice.ReverseFind( _T('\\') );
	strName = strDevice.Mid( pos + 1 );

	int sub = 0;

	int iListItemCount = list.GetItemCount();
	int nPos = list.InsertItem( iListItemCount, _T("") );

	list.SetItemText( nPos, sub+0, strHandle );

	if( bPid )
	{
		strPID.Format( _T("%d"), sh.ProcessID );
		list.SetItemText( nPos, sub+1, strPID );
		sub++;

		strProcesName = GetProcessName( sh.ProcessID );
		list.SetItemText( nPos, sub+1, strProcesName );
		sub++;
	}

	list.SetItemText( nPos, sub+1, strName );
	list.SetItemText( nPos, sub+2, strPath );
	list.SetItemText( nPos, sub+3, strDevice );

	return nPos;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
