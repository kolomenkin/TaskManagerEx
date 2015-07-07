// Written by Zoltan Csizmadia, zoltan_csizmadia@yahoo.com
// For companies(Austin,TX): If you would like to get my resume, send an email.
//
// The source is free, but if you want to use it, mention my name and e-mail address
//
// Modified by Sergey Kolomenkin, register@tut.by, 2005
//
//////////////////////////////////////////////////////////////////////////////////////
//
// SystemInfo.cpp v1.1
//
// History:
//
// Date      Version     Description
// --------------------------------------------------------------------------------
// 10/16/00	 1.0	     Initial version
// 11/09/00  1.1         NT4 doesn't like if we bother the System process fix :)
//                       SystemInfoUtils::GetDeviceFileName() fix (subst drives added)
//                       NT Major version added to INtDLL class
//    03/05  1.2         Added IPsapi, IToolhelp32, some other features added.
//
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <process.h>

#include "SystemInfo.h"

//////////////////////////////////////////////////////////////////////////////////////
// This is for GetLongPathName function on Windows NT4 SP3 and less:
#define		COMPILE_NEWAPIS_STUBS
#define		WANT_GETLONGPATHNAME_WRAPPER
//#include <NewAPIs.h>

/*****************************************************************************
 *
 * GetLongPathName
 *
 *****************************************************************************/

#ifdef WANT_GETLONGPATHNAME_WRAPPER

#include <shlobj.h>

#undef GetLongPathName
#define GetLongPathName _GetLongPathName

extern DWORD (CALLBACK *GetLongPathName)(LPCTSTR, LPTSTR, DWORD);

/*
 * Exactly one file should define this symbol.
 */
#ifdef COMPILE_NEWAPIS_STUBS

/*
 * The version to use if we are forced to emulate.
 */
static DWORD WINAPI
Emulate_GetLongPathName(LPCTSTR ptszShort, LPTSTR ptszLong, DWORD ctchBuf)
{
    LPSHELLFOLDER psfDesk;
    HRESULT hr;
    LPITEMIDLIST pidl;
    TCHAR tsz[MAX_PATH];            /* Scratch TCHAR buffer */
    DWORD dwRc;
    LPMALLOC pMalloc;

    /*
     *  The file had better exist.  GetFileAttributes() will
     *  not only tell us, but it'll even call SetLastError()
     *  for us.
     */
    if (GetFileAttributes(ptszShort) == 0xFFFFFFFF) {
        return 0;
    }

    /*
     *  First convert from relative path to absolute path.
     *  This uses the scratch TCHAR buffer.
     */
    dwRc = GetFullPathName(ptszShort, MAX_PATH, tsz, NULL);
    if (dwRc == 0) {
        /*
         *  Failed; GFPN already did SetLastError().
         */
    } else if (dwRc >= MAX_PATH) {
        /*
         *  Resulting path would be too long.
         */
        SetLastError(ERROR_BUFFER_OVERFLOW);
        dwRc = 0;
    } else {
        /*
         *  Just right.
         */
        hr = SHGetDesktopFolder(&psfDesk);
        if (SUCCEEDED(hr)) {
            ULONG cwchEaten;

#ifdef UNICODE
#ifdef __cplusplus
            hr = psfDesk->ParseDisplayName(NULL, NULL, tsz,
                                       &cwchEaten, &pidl, NULL);
#else
            hr = psfDesk->lpVtbl->ParseDisplayName(psfDesk, NULL, NULL, tsz,
                                       &cwchEaten, &pidl, NULL);
#endif
#else
            WCHAR wsz[MAX_PATH];        /* Scratch WCHAR buffer */

            /*
             *  ParseDisplayName requires UNICODE, so we use
             *  the scratch WCHAR buffer during the conversion.
             */
            dwRc = MultiByteToWideChar(
                        AreFileApisANSI() ? CP_ACP : CP_OEMCP,
                        0, tsz, -1, wsz, MAX_PATH);
            if (dwRc == 0) {
                /*
                 *  Couldn't convert to UNICODE.  MB2WC uses
                 *  ERROR_INSUFFICIENT_BUFFER, which we convert
                 *  to ERROR_BUFFER_OVERFLOW.  Any other error
                 *  we leave alone.
                 */
                if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                    SetLastError(ERROR_BUFFER_OVERFLOW);
                }
                dwRc = 0;
            } else {
#ifdef __cplusplus
                hr = psfDesk->ParseDisplayName(NULL, NULL, wsz,
                                           &cwchEaten, &pidl, NULL);
#else
                hr = psfDesk->lpVtbl->ParseDisplayName(psfDesk, NULL, NULL,
                                           wsz, &cwchEaten, &pidl, NULL);
#endif
#endif

                if (FAILED(hr)) {
                    /*
                     *  Weird.  Convert the result back to a Win32
                     *  error code if we can.  Otherwise, use the
                     *  generic "duh" error code ERROR_INVALID_DATA.
                     */
                    if (HRESULT_FACILITY(hr) == FACILITY_WIN32) {
                        SetLastError(HRESULT_CODE(hr));
                    } else {
                        SetLastError(ERROR_INVALID_DATA);
                    }
                    dwRc = 0;
                } else {
                    /*
                     *  Convert the pidl back to a filename in the
                     *  TCHAR scratch buffer.
                     */
                    dwRc = SHGetPathFromIDList(pidl, tsz);
                    if (dwRc == 0 && tsz[0]) {
                        /*
                         *  Bizarre failure.
                         */
                        SetLastError(ERROR_INVALID_DATA);
                    } else {
                        /*
                         *  Copy the result back to the user's buffer.
                         */
                        dwRc = lstrlen(tsz);
                        if (dwRc + 1 > ctchBuf) {
                            /*
                             *  On buffer overflow, return necessary
                             *  size including terminating null (+1).
                             */
                            SetLastError(ERROR_INSUFFICIENT_BUFFER);
                            dwRc = dwRc + 1;
                        } else {
                            /*
                             *  On buffer okay, return actual size not
                             *  including terminating null.
                             */
                            lstrcpyn(ptszLong, tsz, ctchBuf);
                        }
                    }

                    /*
                     *  Free the pidl.
                     */
                    if (SUCCEEDED(SHGetMalloc(&pMalloc))) {
#ifdef __cplusplus
                        pMalloc->Free(pidl);
                        pMalloc->Release();
#else
                        pMalloc->lpVtbl->Free(pMalloc, pidl);
                        pMalloc->lpVtbl->Release(pMalloc);
#endif
                    }
                }
#ifndef UNICODE
            }
#endif
            /*
             *  Release the desktop folder now that we no longer
             *  need it.
             */
#ifdef __cplusplus
            psfDesk->Release();
#else
            psfDesk->lpVtbl->Release(psfDesk);
#endif
        }
    }
    return dwRc;
}

/*
 * The stub that probes to decide which version to use.
 */
static DWORD WINAPI
Probe_GetLongPathName(LPCTSTR ptszShort, LPTSTR ptszLong, DWORD ctchBuf)
{
    HINSTANCE hinst;
    FARPROC fp;
    DWORD dwRc;
    DWORD (CALLBACK *RealGetLongPathName)(LPCTSTR, LPTSTR, DWORD);

    hinst = GetModuleHandle(TEXT("KERNEL32"));
#ifdef UNICODE
    fp = GetProcAddress(hinst, "GetLongPathNameW");
#else
    fp = GetProcAddress(hinst, "GetLongPathNameA");
#endif

    if (fp) {
        *(FARPROC *)&RealGetLongPathName = fp;
        dwRc = RealGetLongPathName(ptszShort, ptszLong, ctchBuf);
        if (dwRc || GetLastError() != ERROR_CALL_NOT_IMPLEMENTED) {
            GetLongPathName = RealGetLongPathName;
        } else {
            GetLongPathName = Emulate_GetLongPathName;
            dwRc = GetLongPathName(ptszShort, ptszLong, ctchBuf);
        }
    } else {
        GetLongPathName = Emulate_GetLongPathName;
        dwRc = GetLongPathName(ptszShort, ptszLong, ctchBuf);
    }

    return dwRc;

}

DWORD (CALLBACK *GetLongPathName)(LPCTSTR, LPTSTR, DWORD) =
                Probe_GetLongPathName;

#endif /* COMPILE_NEWAPIS_STUBS */
#endif /* WANT_GETLONGPATHNAME_WRAPPER */

//////////////////////////////////////////////////////////////////////////////////////

//#ifndef WINNT
//#error You need Windows NT to use this source code. Define WINNT!
//#endif

static char g_szStamp_Mark[] = "\r\n\r\nSystemInfo.cpp Timestamp: " __DATE__ ", " __TIME__ "\r\n\r\n";

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemInfoUtils
//
//////////////////////////////////////////////////////////////////////////////////////

// From wide char string to CString
void SystemInfoUtils::LPCWSTR2CString( LPCWSTR strW, CString& str )
{
	USES_CONVERSION;
	str = W2CT( strW );

//#ifdef UNICODE
//	// if it is already UNICODE, no problem
//	str = strW;
//#else
//
//	ULONG len = wcslen(strW) + 1;
//	TCHAR* pBuffer = new TCHAR[ len ];
//	if( pBuffer == NULL )
//	{
//		ASSERT( FALSE );
//		str.Empty();
//		return;
//	}
//
//	int res = WideCharToMultiByte( CP_ACP, 0, strW, len, pBuffer, len, NULL, NULL );
//	if( res == 0 )
//	{
//		pBuffer[0] = _T('\0');
//	}
//
//	str = pBuffer;
//
//	delete [] pBuffer;
//#endif
}

// From wide char string to unicode
void SystemInfoUtils::Unicode2CString( UNICODE_STRING* strU, CString& str )
{
	if ( strU->Length != 0 && strU->Buffer != NULL )
		LPCWSTR2CString( (LPCWSTR)strU->Buffer, str );
	else
		str = _T("");
}

// From device file name to DOS filename
BOOL SystemInfoUtils::GetFsFileName( LPCTSTR lpDeviceFileName, CString& fsFileName )
{
	BOOL rc = FALSE;

	TCHAR lpDeviceName[0x1000];
	TCHAR lpDrive[3] = _T("A:");

	// Iterating through the drive letters
	for ( TCHAR actDrive = _T('A'); actDrive <= _T('Z'); actDrive++ )
	{
		lpDrive[0] = actDrive;

		// Query the device for the drive letter
		if ( QueryDosDevice( lpDrive, lpDeviceName, 0x1000 ) != 0 )
		{
			// Network drive?
			if ( _tcsnicmp( _T("\\Device\\LanmanRedirector\\"), lpDeviceName, 25 ) == 0 )
			{
				//Mapped network drive

				char cDriveLetter;
				DWORD dwParam;

				TCHAR lpSharedName[0x1000];

				if ( _stscanf(  lpDeviceName,
								_T("\\Device\\LanmanRedirector\\;%c:%d\\%s"),
								&cDriveLetter,
								&dwParam,
								lpSharedName ) != 3 )
						continue;

				_tcscpy( lpDeviceName, _T("\\Device\\LanmanRedirector\\") );
				_tcscat( lpDeviceName, lpSharedName );
			}

			// Is this the drive letter we are looking for?
			if ( _tcsnicmp( lpDeviceName, lpDeviceFileName, _tcslen( lpDeviceName ) ) == 0 )
			{
				fsFileName = lpDrive;
				fsFileName += (LPCTSTR)( lpDeviceFileName + _tcslen( lpDeviceName ) );

				rc = TRUE;

				break;
			}
		}
	}

	return rc;
}

// From DOS file name to device file name
BOOL SystemInfoUtils::GetDeviceFileName( LPCTSTR lpFsFileName, CString& deviceFileName )
{
	BOOL rc = FALSE;
	TCHAR lpDrive[3];

	// Get the drive letter
	// unfortunetaly it works only with DOS file names
	_tcsncpy( lpDrive, lpFsFileName, 2 );
	lpDrive[2] = _T('\0');

	TCHAR lpDeviceName[0x1000];

	// Query the device for the drive letter
	if ( QueryDosDevice( lpDrive, lpDeviceName, 0x1000 ) != 0 )
	{
		// Subst drive?
		if ( _tcsnicmp( _T("\\??\\"), lpDeviceName, 4 ) == 0 )
		{
			deviceFileName = lpDeviceName + 4;
			deviceFileName += lpFsFileName + 2;

			return TRUE;
		}
		else
		// Network drive?
		if ( _tcsnicmp( _T("\\Device\\LanmanRedirector\\"), lpDeviceName, 25 ) == 0 )
		{
			//Mapped network drive

			char cDriveLetter;
			DWORD dwParam;

			TCHAR lpSharedName[0x1000];

			if ( _stscanf(  lpDeviceName,
							_T("\\Device\\LanmanRedirector\\;%c:%d\\%s"),
							&cDriveLetter,
							&dwParam,
							lpSharedName ) != 3 )
					return FALSE;

			_tcscpy( lpDeviceName, _T("\\Device\\LanmanRedirector\\") );
			_tcscat( lpDeviceName, lpSharedName );
		}

		_tcscat( lpDeviceName, lpFsFileName + 2 );

		deviceFileName = lpDeviceName;

		rc = TRUE;
	}

	return rc;
}

BOOL beginsi( const CString& s, const CString& strBegin )
{
	int len = strBegin.GetLength();
	BOOL res = s.Left( len ).CompareNoCase( strBegin ) == 0;
	return res;
}

CString SystemInfoUtils::DecodeModuleName( const CString& strFullName )
{
	CString s;
	TCHAR szWinSysDir[MAX_PATH] = _T("");
	TCHAR szWinDir[MAX_PATH] = _T("");
	GetSystemDirectory( szWinSysDir, SIZEOF_ARRAY(szWinSysDir) );
	GetWindowsDirectory( szWinDir, SIZEOF_ARRAY(szWinDir) );

	const CString strBeginQuestion = _T("\\??\\");
	const CString strBeginSystemRoot = _T("\\SystemRoot\\");

	int iSlashPos = strFullName.Find( _T('\\') );
	if( iSlashPos == -1 )
	{
		s.Format( _T("%s\\drivers\\%s"), szWinSysDir, strFullName );
	}
	else if( beginsi( strFullName, strBeginQuestion ) )
	{
		s = strFullName.Mid( strBeginQuestion.GetLength() );
	}
	else if( beginsi( strFullName, strBeginSystemRoot ) )
	{
		s.Format( _T("%s\\%s"), szWinDir, strFullName.Mid( strBeginSystemRoot.GetLength() ) );
	}
	else
	{
		if( iSlashPos == 0 )
		{
			s.Format( _T("%c:%s"), szWinDir[0], strFullName );
		}
		else
		{
			s = strFullName;
		}
	}

	CString tmp;

	tmp = szWinDir;
	if( beginsi( s, tmp ) )
	{
		s = tmp + s.Mid( tmp.GetLength() );
	}

	tmp = szWinSysDir;
	if( beginsi( s, tmp ) )
	{
		s = tmp + s.Mid( tmp.GetLength() );
	}

	tmp = szWinSysDir;
	tmp += _T("\\drivers");
	if( beginsi( s, tmp ) )
	{
		s = tmp + s.Mid( tmp.GetLength() );
	}

	return s;
}

//////////////////////////////////////////////////////////////////////////////////////

PVOID Alloc( PVOID pStartAddress, DWORD dwBytes )
{
	pStartAddress = pStartAddress;
	PVOID pAddr = NULL;
	pAddr = new PBYTE[dwBytes];
//	pAddr = VirtualAlloc( NULL, dwBytes, MEM_COMMIT, PAGE_READWRITE );
//	pAddr = VirtualAlloc( pStartAddress, dwBytes, MEM_COMMIT, PAGE_READWRITE );
	return pAddr;
}

void Free( PVOID pAddr )
{
	if( pAddr != NULL )
	{
		delete[] (PBYTE)pAddr;
//		VirtualFree( pAddr, 0, MEM_RELEASE );
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemProcessInformation
//
//////////////////////////////////////////////////////////////////////////////////////

SystemProcessInformation::SystemProcessInformation( DWORD processId, BOOL bAdditionalInfo, BOOL bRefresh )
{
	m_processId = processId;
	m_bAdditionalInfo = bAdditionalInfo;

//	m_pBuffer = (PUCHAR) Alloc( (void*)0x100000, BufferSize ); // (void*)0x100000 is a bug! There were crashes because of it!
	m_pBuffer = (PUCHAR) Alloc( NULL, BufferSize );

	ASSERT( m_pBuffer != NULL );

	if ( bRefresh )
		Refresh();
}

SystemProcessInformation::~SystemProcessInformation()
{
	DWORD pID;
	PROCESS_INFO process_info;

	for ( POSITION pos = m_ProcessInfos.GetStartPosition(); pos != NULL; )
	{
		m_ProcessInfos.GetNextAssoc( pos, pID, process_info );
		if( process_info.pThreads != NULL )
		{
			delete[] process_info.pThreads;
		}
	}
	if( m_pBuffer != NULL )
	{
		Free( m_pBuffer );
	}
}

BOOL SystemProcessInformation::GetAdditionalInfo( PROCESS_INFO& info )
{
	BOOL res;

	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, info.processId );
	if( hProcess != NULL )
	{
		DWORD dwSize = 0;
		NTSTATUS status = INtDll::NtQueryInformationProcess( hProcess, ProcessBasicInformation,
			&info.pi, sizeof(info.pi), &dwSize );
		if( !NT_SUCCESS(status) )
		{
		}

		if( IPsapi::GetProcessMemoryInfo != NULL )
		{
			IPsapi::GetProcessMemoryInfo( hProcess, &info.pmc, sizeof(info.pmc) );
		}

		if( IPsapi::GetModuleFileNameEx != NULL )
		{
			DWORD dwRet = IPsapi::GetModuleFileNameEx( hProcess, NULL, info.szExe, sizeof(info.szExe) );
			info.szExe[dwRet] = _T('\0');
		}

		//res = GetProcessIoCounters( hProcess, &info.ioc ); // Windows 2000+
		res = GetProcessPriorityBoost( hProcess, &info.bDisablePriorityBoost );
		res = GetProcessAffinityMask( hProcess, &info.nProcessAffinity, &info.nSystemAffinity );
		//res = GetProcessDefaultLayout( hProcess, &info.dwDefaultLayout ); // Windows 2000+
		res = GetProcessTimes( hProcess, &info.ftCreation, &info.ftExit, &info.ftKernel, &info.ftUser );
		res = GetProcessWorkingSetSize( hProcess, &info.minWorkSet, &info.maxWorkSet );

		CloseHandle( hProcess );
	}

	info.dwVersion = GetProcessVersion( info.processId );
	return TRUE;
}

BOOL SystemProcessInformation::Refresh()
{
	{ // Free used memory before m_ProcessInfos.RemoveAll(); :
		DWORD pID;
		PROCESS_INFO process_info;

		for ( POSITION pos = m_ProcessInfos.GetStartPosition(); pos != NULL; )
		{
			m_ProcessInfos.GetNextAssoc( pos, pID, process_info );
			if( process_info.pThreads != NULL )
			{
				delete[] process_info.pThreads;
			}
		}
	}
	m_ProcessInfos.RemoveAll();

	if ( !INtDll::bStatus )
		return FALSE;

	if ( m_pBuffer == NULL )
		return FALSE;
	
	// query the process information
	NTSTATUS status = INtDll::NtQuerySystemInformation( _SystemProcessInformation, m_pBuffer, BufferSize, NULL );
	if ( !NT_SUCCESS(status) )
		return FALSE;

	SYSTEM_PROCESS_INFORMATION* pSysProcess = (SYSTEM_PROCESS_INFORMATION*)m_pBuffer;
	do
	{
		if( pSysProcess->ProcessId == m_processId || m_processId == ALL_PROCESSES )
		{
			// fill the process information map
			PROCESS_INFO new_info;
			ZeroMemory( &new_info, sizeof(new_info) );
			new_info.processId = pSysProcess->ProcessId;
			new_info.spi = *pSysProcess;
			new_info.pThreads = new SYSTEM_THREAD_INFORMATION[pSysProcess->ThreadCount];
			if( new_info.pThreads != NULL )
			{
				memcpy( new_info.pThreads, &pSysProcess->Threads,
					pSysProcess->ThreadCount * sizeof(SYSTEM_THREAD_INFORMATION) );
			}

			if( m_bAdditionalInfo )
			{
				GetAdditionalInfo( new_info );
			}

			m_ProcessInfos.SetAt( new_info.processId, new_info );
		}

		// get the next process information block
		if ( pSysProcess->NextEntryDelta != 0 )
		{
			{
				// Check new pointer for valid range:
				ULONG_PTR p = (ULONG_PTR)((UCHAR*)pSysProcess + pSysProcess->NextEntryDelta );
				ULONG_PTR sz = sizeof(SYSTEM_PROCESS_INFORMATION);
				ULONG_PTR a = (ULONG_PTR) m_pBuffer;
				ULONG_PTR b = (ULONG_PTR) ((UCHAR*)m_pBuffer + BufferSize);

				ASSERT( a <= p && p+sz <= b );
			}

			pSysProcess = (SYSTEM_PROCESS_INFORMATION*)((UCHAR*)pSysProcess + pSysProcess->NextEntryDelta );
		}
		else
		{
			pSysProcess = NULL;
		}

	} while ( pSysProcess != NULL );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemThreadInformation
//
//////////////////////////////////////////////////////////////////////////////////////

SystemThreadInformation::SystemThreadInformation( DWORD pID, BOOL bRefresh )
{
	m_processId = pID;

	if ( bRefresh )
		Refresh();
}

// Returns the HMODULE that contains the specified memory address
BOOL SystemThreadInformation::ModuleFromAddressEx( DWORD processId, PVOID pv, LPTSTR szModuleName, int cbSize )
{
	if( !IPsapi::bStatus )
		return FALSE;

	//TRACE( _T("ModuleFromAddressEx( PID = %d, addr = 0x%X\n"), processId, pv );

	BOOL result = FALSE;
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId );
	MEMORY_BASIC_INFORMATION mbi;
	ZeroMemory( &mbi, sizeof(mbi) );
	size_t nRet = 0;

	if ( hProcess == NULL )
		goto cleanup;
	//TRACE( _T("ModuleFromAddressEx: process opened!\n") );

	nRet = VirtualQueryEx( hProcess, pv, &mbi, sizeof(mbi));
	if( nRet != sizeof(mbi) )
		goto cleanup;

	//TRACE( _T("ModuleFromAddressEx: module address: 0x%X\n"), mbi.AllocationBase );
	nRet = IPsapi::GetModuleFileNameEx( hProcess, (HMODULE) mbi.AllocationBase, szModuleName, cbSize );
	szModuleName[nRet] = _T('\0');
	if( nRet == 0 )
		goto cleanup;

	result = TRUE;

cleanup:
	if( hProcess != NULL )
	{
		CloseHandle( hProcess );
	}
	return result;
}

BOOL SystemThreadInformation::Refresh()
{
	POSITION pos;
	m_ThreadInfos.RemoveAll();

	// Get the process list
	SystemProcessInformation pi( m_processId, FALSE, TRUE );

	DWORD pID;
	SystemProcessInformation::PROCESS_INFO process_info;

	// Iterating through the processes and get the module list
	for ( pos = pi.m_ProcessInfos.GetStartPosition(); pos != NULL; )
	{
		pi.m_ProcessInfos.GetNextAssoc( pos, pID, process_info );

		SYSTEM_PROCESS_INFORMATION& spi = process_info.spi;
		SYSTEM_THREAD_INFORMATION* pThreads = process_info.pThreads;

		SystemKernelModuleInformation kmi( FALSE );
		if( IS_PID_SYSTEM(pID) )
		{
			kmi.Refresh();
		}

		for( DWORD i=0; i<spi.ThreadCount; i++ )
		{
			SYSTEM_THREAD_INFORMATION& sti = pThreads[i];

			THREAD_INFORMATION ti;
			ZeroMemory( &ti, sizeof(ti) );
			ti.ProcessId = pID;
			ti.ThreadHandle = NULL;
			ti.HandleProcessId = 0;
			ti.sti = sti;

			if( IS_PID_SYSTEM(pID) )
			{
				// If process is [System] then look for kernel modules:
				PVOID pv = ti.sti.StartAddress;
				LPTSTR szModuleName = ti.Module;
				int cbSize = SIZEOF_ARRAY(ti.Module);

				SystemKernelModuleInformation::KERNEL_MODULE_INFORMATION kmBest;
				ptrdiff_t BestFuncOffset = PTRDIFF_T_MAX;

				for ( POSITION pos = kmi.m_KernelModuleInfos.GetHeadPosition(); pos != NULL; )
				{
					SystemKernelModuleInformation::KERNEL_MODULE_INFORMATION& km = kmi.m_KernelModuleInfos.GetNext(pos);

					ptrdiff_t iFuncOffset = (PBYTE)pv - (PBYTE)km.pBaseAddress;
					if( iFuncOffset >= 0 && iFuncOffset < BestFuncOffset )
					{
						BestFuncOffset = iFuncOffset;
						kmBest = km;
					}
				}

				if (BestFuncOffset == PTRDIFF_T_MAX)
				{
					szModuleName[0] = _T('\0');
				}
				else
				{
					_tcsncpy( szModuleName, kmBest.FullPath, cbSize );
				}
			}
			else
			{
				// If process runs in user-mode then get start address module name:
				ModuleFromAddressEx( pID, ti.sti.StartAddress, ti.Module, SIZEOF_ARRAY(ti.Module) );
			}

			m_ThreadInfos.AddTail( ti );
		}
	}


	// Get the Thread objects ( set the filter to "Thread" )
	SystemHandleInformation hi( ALL_PROCESSES, TRUE, _T("Thread") );

	//TRACE( _T("SystemThreadInformation> SystemHandleInformation found %Id threads\n"), hi.m_HandleInfos.GetCount() );

	// Iterating through the found Thread objects
	for ( pos = hi.m_HandleInfos.GetHeadPosition(); pos != NULL; )
	{
		SystemHandleInformation::HANDLE_INFORMATION& h = hi.m_HandleInfos.GetNext(pos);
		DWORD ThreadId = 0;

		// This is one of the threads we are looking for
		BOOL res = SystemHandleInformation::GetThreadId( (HANDLE)h.sh.HandleNumber, ThreadId, h.sh.ProcessID );

		if( h.sh.ProcessID == m_processId || m_processId == ALL_PROCESSES )
		{
			//TRACE( _T("PID %d: Thread ID: 0x%X\n"), m_processId, ThreadId );
		}

		if ( res )
		{
			for ( POSITION pos2 = m_ThreadInfos.GetHeadPosition(); pos2 != NULL; )
			{
				POSITION LastPos = pos2;
				THREAD_INFORMATION ti = m_ThreadInfos.GetNext(pos2);
				if( ti.sti.ClientId.UniqueThread == ThreadId )
				{
					if( ti.HandleProcessId == ti.ProcessId )
					{
						break; // We have found the handle for this thread and it belongs to the thread's process.
						// If it doesn't belongs, then we will continue searching such (best) handles...
					}
					ti.HandleProcessId = h.sh.ProcessID;
					ti.ThreadHandle = (HANDLE)h.sh.HandleNumber;
					m_ThreadInfos.SetAt( LastPos, ti );

					break;
				}
			}
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemHandleInformation
//
//////////////////////////////////////////////////////////////////////////////////////

SystemHandleInformation::SystemHandleInformation( DWORD pID, BOOL bRefresh, LPCTSTR lpTypeFilter )
{
	m_processId = pID;

	// Set the filter
	SetFilter( lpTypeFilter, bRefresh );
}

SystemHandleInformation::~SystemHandleInformation()
{
}

BOOL SystemHandleInformation::SetFilter( LPCTSTR lpTypeFilter, BOOL bRefresh )
{
	// Set the filter ( default = all objects )
	m_strTypeFilter = lpTypeFilter == NULL ? _T("") : lpTypeFilter;

	return bRefresh ? Refresh() : TRUE;
}

const CString& SystemHandleInformation::GetFilter()
{
	return m_strTypeFilter;
}

BOOL SystemHandleInformation::IsSupportedHandle( SYSTEM_HANDLE& handle )
{
	//Here you can filter the handles you don't want in the Handle list

	handle; // use variable

	// Windows 2000 supports everything :)
	return TRUE;
}

BOOL SystemHandleInformation::Refresh()
{
	DWORD size = 0x2000;
	DWORD needed = 0;
	DWORD i = 0;
	BOOL  ret = TRUE;
	CString strType;

	m_HandleInfos.RemoveAll();

	if ( !INtDll::bStatus )
		return FALSE;

	// Allocate the memory for the buffer
	SYSTEM_HANDLE_INFORMATION* pSysHandleInformation = (SYSTEM_HANDLE_INFORMATION*)
				Alloc( NULL, size );

	if ( pSysHandleInformation == NULL )
		return FALSE;

	// Query the needed buffer size for the objects ( system wide )
	NTSTATUS status = INtDll::NtQuerySystemInformation( _SystemHandleInformation, pSysHandleInformation, size, &needed );
	if ( !NT_SUCCESS(status) )
	{
		if ( needed == 0 )
		{
			ret = FALSE;
			goto cleanup;
		}

		// The size was not enough
		Free( pSysHandleInformation );

		size = needed + 1024 * sizeof(SYSTEM_HANDLE);

		pSysHandleInformation = (SYSTEM_HANDLE_INFORMATION*)
				Alloc( NULL, size );
	}

	if ( pSysHandleInformation == NULL )
		return FALSE;

	// Query the objects ( system wide )
	status = INtDll::NtQuerySystemInformation( _SystemHandleInformation, pSysHandleInformation, size, NULL );
	if ( !NT_SUCCESS(status) )
	{
		ret = FALSE;
		goto cleanup;
	}

	//TRACE( _T("SystemHandleInformation::Refresh got %d handles!\n"), pSysHandleInformation->Count );

	// Iterating through the objects
	for ( i = 0; i < pSysHandleInformation->Count; i++ )
	{
		SYSTEM_HANDLE& sh = pSysHandleInformation->Handles[i];
		if ( !IsSupportedHandle( sh ) )
			continue;

		// ProcessId filtering check
		if ( sh.ProcessID == m_processId || m_processId == ALL_PROCESSES )
		{
			BOOL bAdd = FALSE;

			if ( m_strTypeFilter == _T("") )
				bAdd = TRUE;
			else
			{
				// Type filtering
				GetTypeToken( (HANDLE)sh.HandleNumber, strType, sh.ProcessID  );

				bAdd = strType == m_strTypeFilter;
			}

			// That's it. We found one.
			if ( bAdd )
			{
//				sh.HandleType = (WORD)(sh.HandleType % 256);

				HANDLE_INFORMATION hi;
				hi.sh = sh;
				m_HandleInfos.AddTail( hi );

			}
		}
	}

	//TRACE( _T("SystemHandleInformation::Refresh selected %d handles!\n"), m_HandleInfos.GetCount() );

cleanup:

	if ( pSysHandleInformation != NULL )
		Free( pSysHandleInformation );

	return ret;
}

HANDLE SystemHandleInformation::OpenProcess( DWORD processId )
{
	// Open the process for handle duplication
	return ::OpenProcess( PROCESS_DUP_HANDLE, TRUE, processId );
}

HANDLE SystemHandleInformation::DuplicateHandle( HANDLE hProcess, HANDLE hRemote )
{
	HANDLE hDup = NULL;

	// Duplicate the remote handle for our process
	::DuplicateHandle( hProcess, hRemote,	GetCurrentProcess(), &hDup,	0, FALSE, DUPLICATE_SAME_ACCESS ); // works better!
//	::DuplicateHandle( hProcess, hRemote,	GetCurrentProcess(), &hDup,	MAXIMUM_ALLOWED, FALSE, 0 );

	return hDup;
}

NTSTATUS MyNtQueryObject(
			IN	HANDLE				ObjectHandle OPTIONAL,
			IN	DWORD				ObjectInformationClass,
			OUT	PVOID				ObjectInformation,
			IN	ULONG				Length,
			OUT	PULONG				ResultLength )
{
	NTSTATUS status = INtDll::NtQueryObject( ObjectHandle,
		(INtDll::OBJECT_INFORMATION_CLASS)ObjectInformationClass,
		ObjectInformation, Length, ResultLength );
	return status;
}


//Information functions
BOOL SystemHandleInformation::GetTypeToken( HANDLE h, CString& str, DWORD processId )
{
	ULONG size = 0x2000;
	POBJECT_TYPE_INFORMATION lpBuffer = NULL;
	BOOL ret = FALSE;
	NTSTATUS status = STATUS_SUCCESS;

	HANDLE handle = NULL;
	BOOL remote = processId != GetCurrentProcessId();

	if ( !INtDll::bStatus )
		return FALSE;

	if ( remote )
	{
		// Duplicate the handle
		handle = DuplicateHandle( processId, h );

		if( handle == NULL )
			goto cleanup;
	}
	else
		handle = h;

	//TRACE( _T("INtDll::NtQueryObject enter ObjectTypeInformation size = %d\n"), 0 );
	// Query the info size
	status = MyNtQueryObject( handle, ObjectTypeInformation, NULL, 0, &size );
	//TRACE( _T("INtDll::NtQueryObject leave ObjectTypeInformation size = %d\n"), size );

	lpBuffer = (POBJECT_TYPE_INFORMATION) new UCHAR[size];
	if( lpBuffer == NULL )
	{
		goto cleanup;
	}

	// Query the info size ( type )
	//TRACE( _T("INtDll::NtQueryObject enter ObjectTypeInformation size = %d\n"), size );
	status = MyNtQueryObject( handle, ObjectTypeInformation, lpBuffer, size, &size );
	//TRACE( _T("INtDll::NtQueryObject leave ObjectTypeInformation size = %d\n"), size );
	if ( NT_SUCCESS(status) )
	{
		str = _T("");
		SystemInfoUtils::Unicode2CString( &lpBuffer->TypeName, str );

		ret = TRUE;
	}

cleanup:
	if ( remote )
	{
		if ( handle != NULL )
			CloseHandle( handle );
	}

	if ( lpBuffer != NULL )
		delete [] lpBuffer;

	return ret;
}

BOOL SystemHandleInformation::GetType( HANDLE h, OB_TYPE_ENUM& type, DWORD processId )
{
	CString strType;

	type = OB_TYPE_UNKNOWN;

	if ( !GetTypeToken( h, strType, processId ) )
		return FALSE;

	BOOL res = DecodeWinObjectType( strType, type );
	return res;
}

BOOL SystemHandleInformation::GetName( HANDLE handle, CString& str, DWORD processId )
{
	OB_TYPE_ENUM type = OB_TYPE_UNKNOWN;

	if ( !GetType( handle, type, processId  ) )
		return FALSE;

	BOOL res = GetNameByType( handle, type, str, processId );
	return res;
}

BOOL SystemHandleInformation::GetNameByType( HANDLE h, OB_TYPE_ENUM type, CString& str, DWORD processId )
{
	ULONG size = 0x2000;
	POBJECT_NAME_INFORMATION lpBuffer = NULL;
	BOOL ret = FALSE;
	NTSTATUS status = STATUS_SUCCESS;

	HANDLE handle = NULL;
	BOOL remote = processId != GetCurrentProcessId();
	DWORD dwId = 0;

	if ( !INtDll::bStatus )
		return FALSE;

	if ( remote )
	{
		// Duplicate the handle
		handle = DuplicateHandle( processId, h );
		if( handle == NULL )
			goto cleanup;
	}
	else
		handle = h;

	// let's be happy, handle is in our process space, so query the infos :)
	switch( type )
	{
	case OB_TYPE_PROCESS:
		GetProcessId( handle, dwId );

		str.Format( _T("PID: %d (0x%X)"), dwId, dwId );

		ret = TRUE;
		goto cleanup;
		break;

	case OB_TYPE_THREAD:
		GetThreadId( handle, dwId );

		str.Format( _T("TID: %d (0x%X)"), dwId, dwId );

		ret = TRUE;
		goto cleanup;
		break;

	case OB_TYPE_FILE:

		// Check for access only! If these is no access, then it is possible
		// to hung up in NtQueryObject (ObjectNameInformation):
		ret = GetFileName( handle, str );

		// access denied :(
		if ( ret == 2 /*special code*/ && str == _T("") )
		{
			str = _T("\"[access denied!]\"");
			goto cleanup;
		}

		break;

	};

	//TRACE( _T("INtDll::NtQueryObject enter ObjectNameInformation size = %d\n"), 0 );
	status = MyNtQueryObject ( handle, ObjectNameInformation, NULL, 0, &size );
	//TRACE( _T("INtDll::NtQueryObject leave ObjectNameInformation size = %d\n"), size );

	// let's try to use the default
	if ( size == 0 )
		size = 0x2000;

	lpBuffer = (POBJECT_NAME_INFORMATION) new UCHAR[size];
	if( lpBuffer == NULL )
	{
		goto cleanup;
	}

	//TRACE( _T("INtDll::NtQueryObject enter ObjectNameInformation size = %d\n"), size );
	status = MyNtQueryObject( handle, ObjectNameInformation, lpBuffer, size, &size );
	//TRACE( _T("INtDll::NtQueryObject leave ObjectNameInformation size = %d\n"), size );
	if ( NT_SUCCESS(status) )
	{
		SystemInfoUtils::Unicode2CString( &lpBuffer->ObjectName, str );
		ret = TRUE;
	}

cleanup:

	if ( remote )
	{
		if ( handle != NULL )
			CloseHandle( handle );
	}

	if ( lpBuffer != NULL )
		delete [] lpBuffer;

	return ret;
}

//Thread related functions
BOOL SystemHandleInformation::GetThreadId( HANDLE h, DWORD& threadID, DWORD processId )
{
	BASIC_THREAD_INFORMATION ti;
	HANDLE handle;
	BOOL remote = processId != GetCurrentProcessId();

	if ( !INtDll::bStatus )
		return FALSE;

	if ( remote )
	{
		// Duplicate the handle
		handle = DuplicateHandle( processId, h );
	}
	else
		handle = h;

	// Get the thread information
	NTSTATUS status = INtDll::NtQueryInformationThread( handle, ThreadBasicInformation, &ti, sizeof(ti), NULL );
	if ( NT_SUCCESS(status) )
		threadID = ti.ThreadId;

	if ( remote )
	{
		if ( handle != NULL )
			CloseHandle( handle );
	}

	return TRUE;
}

//Process related functions
BOOL SystemHandleInformation::GetProcessPath( HANDLE h, CString& strPath, DWORD remoteProcessId )
{
	h; strPath; remoteProcessId;

	strPath.Format( _T("%d"), remoteProcessId );

	return TRUE;
}

BOOL SystemHandleInformation::GetProcessId( HANDLE h, DWORD& processId, DWORD remoteProcessId )
{
	BOOL ret = FALSE;
	HANDLE handle;
	BOOL remote = remoteProcessId != GetCurrentProcessId();
	PROCESS_BASIC_INFORMATION pi;

	ZeroMemory( &pi, sizeof(pi) );
	processId = 0;

	if ( !INtDll::bStatus )
		return FALSE;

	if ( remote )
	{
		// Duplicate the handle
		handle = DuplicateHandle( processId, h );
	}
	else
		handle = h;

	// Get the process information
	NTSTATUS status = INtDll::NtQueryInformationProcess( handle, ProcessBasicInformation, &pi, sizeof(pi), NULL);
	if ( NT_SUCCESS(status) )
	{
		processId = pi.UniqueProcessId;
		ret = TRUE;
	}

	if ( remote )
	{
		if ( handle != NULL )
			CloseHandle( handle );
	}

	return ret;
}

//File related functions
UINT __stdcall SystemHandleInformation::GetFileNameThread( PVOID pParam )
{
	// This thread function for getting the filename
	// if access denied, we hang up in this function,
	// so if it times out we just kill this thread

	GetFileNameThreadParam* p = (GetFileNameThreadParam*)pParam;

	UCHAR Buffer[0x1000];
	PFILE_NAME_INFORMATION lpBuffer = (PFILE_NAME_INFORMATION) &Buffer;
	IO_STATUS_BLOCK iob;

	p->rc = INtDll::NtQueryInformationFile( p->hFile, &iob, lpBuffer, sizeof(Buffer), FileNameInformation );

	if ( NT_SUCCESS(p->rc) )
	{
		*p->pName = lpBuffer->FileName;
	}

	//OutputDebugString( _T("NtQueryInformationFile: ") );
	//OutputDebugString( *p->pName );
	//OutputDebugString( _T("\n") );
    _endthreadex( 0 );
	return 0;
}

BOOL SystemHandleInformation::GetFileName( HANDLE h, CString& str, DWORD processId )
{
	BOOL ret= FALSE;
	HANDLE hThread = NULL;
	GetFileNameThreadParam tp;
	HANDLE handle = NULL;
	BOOL remote = processId != GetCurrentProcessId();

	if ( !INtDll::bStatus )
		return FALSE;

	if ( remote )
	{
		// Duplicate the handle
		handle = DuplicateHandle( processId, h );
	}
	else
		handle = h;

	tp.hFile = handle;
	tp.pName = &str;
	tp.rc = 0;

	// Let's start the thread to get the file name
	hThread = (HANDLE)_beginthreadex( NULL, 0, GetFileNameThread, &tp, 0, NULL );

	if ( hThread == NULL )
	{
		ret = FALSE;
		goto cleanup;
	}

	// Wait for finishing the thread
	if ( WaitForSingleObject( hThread, 100 ) == WAIT_TIMEOUT )
	{
		TRACE( _T("TerminateThread( 0x%X, 0 ) - Access denied!\n"), hThread );
		// Access denied
		// Terminate the thread
		TerminateThread( hThread, 0 );

		str = _T("");

		ret = 2; // more than TRUE 8-)
	}
	else
		ret = NT_SUCCESS(tp.rc);

cleanup:

	if( hThread != NULL )
	{
		CloseHandle( hThread );
	}

	if ( remote )
	{
		if ( handle != NULL )
			CloseHandle( handle );
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemModuleInformation
//
//////////////////////////////////////////////////////////////////////////////////////

SystemModuleInformation::SystemModuleInformation( DWORD pID, BOOL bRefresh )
{
	m_processId = pID;

	if ( bRefresh )
		Refresh();
}

void SystemModuleInformation::GetModuleListForProcess( DWORD processID )
{
	DWORD i = 0;
	DWORD cbNeeded = 0;
	HMODULE* hModules = NULL;
	MODULE_INFO moduleInfo;
	HANDLE hSnapshot = INVALID_HANDLE_VALUE;
	DWORD dwModules = 0;

	// Open process to read to query the module list
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID );

	if ( hProcess == NULL )
		goto cleanup;

	//Get the number of modules
	if ( !IPsapi::EnumProcessModules( hProcess, NULL, 0, &cbNeeded ) )
		goto cleanup;

	cbNeeded += 50 * sizeof( HMODULE ); // extra
	hModules = new HMODULE[ cbNeeded / sizeof( HMODULE ) ];
	if( hModules == NULL )
		goto cleanup;

	//Get module handles
    if ( !IPsapi::EnumProcessModules( hProcess, hModules, cbNeeded, &cbNeeded ) )
		goto cleanup;

	if( IToolhelp32::bStatus != FALSE && processID != 0 )
	{
		hSnapshot = IToolhelp32::CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, processID );
	}

	dwModules = cbNeeded / sizeof( HMODULE );

	//TRACE( _T("SystemModuleInformation::GetModuleListForProcess got %d modules!\n"), dwModules );

	for ( i = 0; i < dwModules; i++ )
	{
		ZeroMemory( &moduleInfo, sizeof(moduleInfo) );
		moduleInfo.ProcessId = processID;
		moduleInfo.Handle = hModules[i];

		DWORD dwRet1, dwRet2, dwRet3;
		BOOL res;
		dwRet1 = VirtualQueryEx( hProcess, hModules[i], &moduleInfo.mbi, sizeof(moduleInfo.mbi) );
		res = IPsapi::GetModuleInformation( hProcess, hModules[i], &moduleInfo.info, sizeof(moduleInfo.info) );
		dwRet3 = IPsapi::GetModuleBaseName( hProcess, hModules[i], moduleInfo.Name, SIZEOF_ARRAY(moduleInfo.Name) );
		moduleInfo.Name[dwRet3] = _T('\0');
		dwRet2 = IPsapi::GetModuleFileNameEx( hProcess, hModules[i], moduleInfo.FullPath, SIZEOF_ARRAY(moduleInfo.FullPath) );
		moduleInfo.FullPath[dwRet2] = _T('\0');

		CString strFullName = SystemInfoUtils::DecodeModuleName( CString(moduleInfo.FullPath) );
		_tcsncpy( moduleInfo.FullPath, strFullName, sizeof(moduleInfo.FullPath)/sizeof(moduleInfo.FullPath[0]) );

		res = SystemMemoryMapInformation::FileFromAddress( processID, hModules[i], moduleInfo.FileFullPath, SIZEOF_ARRAY(moduleInfo.FileFullPath) );

		if( hSnapshot != INVALID_HANDLE_VALUE )
		{
			MODULEENTRY32 me32 = {0};
			me32.dwSize = sizeof(me32);

			for( res = IToolhelp32::Module32First( hSnapshot, &me32 ); res != FALSE; res = IToolhelp32::Module32Next( hSnapshot, &me32 ) )
			{
				if( me32.hModule == hModules[i] )
				{
					ASSERT( me32.th32ProcessID == processID );
					moduleInfo.me32 = me32;
					break;
				}
			}
		}

		//Get module full paths
		//if ( dwRet2 && dwRet3 )
			m_ModuleInfos.AddTail( moduleInfo );
	}

cleanup:

	if( hSnapshot != INVALID_HANDLE_VALUE )
	{
		CloseHandle( hSnapshot );
	}

	if ( hModules != NULL )
		delete [] hModules;

	if ( hProcess != NULL )
		CloseHandle( hProcess );
}

BOOL SystemModuleInformation::Refresh()
{
	if ( !IPsapi::bStatus )
		return FALSE;

	m_ModuleInfos.RemoveAll();

	// Everey process or just a particular one
	if ( m_processId != ALL_PROCESSES )
		// For a particular one
		GetModuleListForProcess( m_processId );
	else
	{
/* I think this is not the simpliest way: (Sergey Kolomenkin)
		// Get teh process list
		DWORD pID;
		SystemProcessInformation::PROCESS_INFO process_info;
		SystemProcessInformation pi( TRUE );

		if ( pi.m_ProcessInfos.GetStartPosition() == NULL )
		{
			return FALSE;
		}

		// Iterating through the processes and get the module list
		for ( POSITION pos = pi.m_ProcessInfos.GetStartPosition(); pos != NULL; )
		{
			pi.m_ProcessInfos.GetNextAssoc( pos, pID, process_info );
			GetModuleListForProcess( pID );
		}
*/
		// I think my way would be more convenient (Sergey Kolomenkin)

		DWORD cbNeeded = 0;
		DWORD* pProcesses = NULL;

		//Get the number of modules
		if ( !IPsapi::EnumProcesses( NULL, 0, &cbNeeded ) )
			return FALSE;

		cbNeeded += 50 * sizeof( DWORD ); // extra

		pProcesses = new DWORD[ cbNeeded / sizeof( DWORD ) ];
		if( pProcesses == NULL )
			return FALSE;

		//Get module handles
		if ( !IPsapi::EnumProcesses( pProcesses, cbNeeded, &cbNeeded ) )
		{
			delete [] pProcesses;
			return FALSE;
		}

		DWORD dwProcesses = cbNeeded / sizeof( DWORD );

		//TRACE( _T("SystemModuleInformation::Refresh got %d processes!\n"), dwProcesses );

		for( DWORD i=0; i<dwProcesses; i++ )
		{
			GetModuleListForProcess( pProcesses[i] );
		}

		delete [] pProcesses;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemKernelModuleInformation
//
//////////////////////////////////////////////////////////////////////////////////////

SystemKernelModuleInformation::SystemKernelModuleInformation( BOOL bRefresh )
{
	if ( bRefresh )
		Refresh();
}

BOOL SystemKernelModuleInformation::Refresh()
{
	if ( !IPsapi::bStatus )
		return FALSE;

	m_KernelModuleInfos.RemoveAll();

	DWORD cbNeeded = 0;
	LPVOID* pBaseAddresses = NULL;

	//Get the number of modules
	if ( !IPsapi::EnumDeviceDrivers( NULL, 0, &cbNeeded ) )
		return FALSE;

	cbNeeded += 50 * sizeof( LPVOID ); // extra

	pBaseAddresses = new LPVOID[ cbNeeded / sizeof( LPVOID ) ];
	if( pBaseAddresses == NULL )
		return FALSE;

	//Get module handles
	if ( !IPsapi::EnumDeviceDrivers( pBaseAddresses, cbNeeded, &cbNeeded ) )
	{
		delete [] pBaseAddresses;
		return FALSE;
	}

	DWORD dwKernelModules = cbNeeded / sizeof( LPVOID );

	//TRACE( _T("SystemKernelModuleInformation::Refresh got %d kernel modules!\n"), dwKernelModules );

	for( DWORD i=0; i<dwKernelModules; i++ )
	{
		KERNEL_MODULE_INFORMATION kmi;
		ZeroMemory( &kmi, sizeof(kmi) );
		kmi.pBaseAddress = pBaseAddresses[i];
		IPsapi::GetDeviceDriverBaseName( pBaseAddresses[i], kmi.Name, sizeof(kmi.Name)/sizeof(kmi.Name[0]) );
		IPsapi::GetDeviceDriverFileName( pBaseAddresses[i], kmi.FullPath, sizeof(kmi.FullPath)/sizeof(kmi.FullPath[0]) );
		
		m_KernelModuleInfos.AddTail( kmi );
	}

	delete [] pBaseAddresses;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemMemoryMapInformation
//
//////////////////////////////////////////////////////////////////////////////////////

SystemMemoryMapInformation::SystemMemoryMapInformation( DWORD pID, BOOL bExpandRegions, BOOL bRefresh )
{
	m_processId = pID;

	if ( bRefresh )
		Refresh( bExpandRegions );
}

///////////////////////////////////////////////////////////////////////////////

BOOL SystemMemoryMapInformation::FileFromAddress( DWORD processId, PVOID pv, LPTSTR szFileName, int cbSize )
{
	if( !IPsapi::bStatus )
		return FALSE;

	//TRACE( _T("FileFromAddress( PID = %d, addr = 0x%X\n"), processId, pv );

	BOOL result = FALSE;
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId );
	DWORD dwRet;
	CString fsFileName;
	IPsapi::MODULEINFO info;
	MEMORY_BASIC_INFORMATION mbi;
	BOOL res;

	if ( hProcess == NULL )
		goto cleanup;
	//TRACE( _T("FileFromAddress: process opened!\n") );

	if( pv == NULL ) // Looking for EXE instance:
	{
		ZeroMemory( &info, sizeof(info) );
		ZeroMemory( &mbi, sizeof(mbi) );
		res = IPsapi::GetModuleInformation( hProcess, NULL, &info, sizeof(info) );
		if( !res )
			goto cleanup;

		dwRet = VirtualQueryEx( hProcess, info.EntryPoint, &mbi, sizeof(mbi));
		if( dwRet != sizeof(mbi) )
			goto cleanup;

		pv = mbi.AllocationBase;
	}

	TRACE( _T("FileFromAddress - 0x%X\n"), pv );

	dwRet = IPsapi::GetMappedFileName( hProcess, pv,
		szFileName, cbSize );
	szFileName[dwRet] = _T('\0');

	TRACE( _T("FileFromAddress - 0x%X - GetMappedFileName returned %d, err = %d\n"), pv, dwRet, GetLastError() );

#if 0
	ERROR_UNEXP_NET_ERR
	ERROR_FILE_INVALID
#endif

	if( dwRet == 0 )
		goto cleanup;

	SystemInfoUtils::GetFsFileName( szFileName, fsFileName );
	GetLongPathName( fsFileName, szFileName, cbSize );

	result = TRUE;

cleanup:
	if( hProcess != NULL )
	{
		CloseHandle( hProcess );
	}
	return result;
}

///////////////////////////////////////////////////////////////////////////////

BOOL SystemMemoryMapInformation::AddMemoryInfo( DWORD processId, HANDLE hProcess, BOOL bRegion, PVMQUERY pVmq )
{
	if ( !IPsapi::bStatus )
		return FALSE;

	MEMORY_INFORMATION mi;
	ZeroMemory( &mi, sizeof(mi) );
	mi.bRegion = bRegion;
	mi.vmq = *pVmq;

	if( bRegion && pVmq->pvRgnBaseAddress != NULL )
	{
		DWORD dwRet = IPsapi::GetModuleFileNameEx( hProcess, (HMODULE)pVmq->pvRgnBaseAddress,
			mi.Module, SIZEOF_ARRAY(mi.Module) );
		mi.Module[dwRet] = _T('\0');

		//if( dwRet == 0 )
		{
			FileFromAddress( processId, pVmq->pvRgnBaseAddress, mi.MappedFile, SIZEOF_ARRAY(mi.MappedFile) );

			if( _tcsicmp( mi.Module, mi.MappedFile ) == 0 )
			{
				mi.MappedFile[0] = _T('\0');
			}
		}
	}

	m_MemoryInfos.AddTail( mi );

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

BOOL SystemMemoryMapInformation::Refresh( BOOL bExpandRegions )
{
	if ( !IPsapi::bStatus )
		return FALSE;

	m_MemoryInfos.RemoveAll();

	// Is the process still running?
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, m_processId);
	if (hProcess == NULL)
	{
		return FALSE;
	}

	// Walk the virtual address space, adding entries to the list box.
	BOOL fOk = TRUE;
	PVOID pvAddress = NULL;

	while (fOk)
	{
		VMQUERY vmq;
		fOk = VMQuery(hProcess, pvAddress, &vmq);

		if (fOk)
		{
			AddMemoryInfo( m_processId, hProcess, TRUE, &vmq );

			if (bExpandRegions)
			{
				for (DWORD dwBlock = 0; fOk && (dwBlock < vmq.dwRgnBlocks);	dwBlock++)
				{
					AddMemoryInfo( m_processId, hProcess, FALSE, &vmq );

					// Get the address of the next region to test.
					pvAddress = ((PBYTE) pvAddress + vmq.BlkSize);
					if (dwBlock < vmq.dwRgnBlocks - 1)
					{
						// Don't query the memory info after the last block.
						fOk = VMQuery(hProcess, pvAddress, &vmq);
					}
				}
			}

			// Get the address of the next region to test.
			pvAddress = ((PBYTE) vmq.pvRgnBaseAddress + vmq.RgnSize);
		}
	}

	CloseHandle(hProcess);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemWindowInformation
//
//////////////////////////////////////////////////////////////////////////////////////

SystemWindowInformation::SystemWindowInformation( DWORD pID, BOOL bRefresh )
{
	m_processId = pID;

	if ( bRefresh )
		Refresh();
}

BOOL SystemWindowInformation::Refresh()
{
	m_WindowInfos.RemoveAll();

	// Enumerating the windows
	EnumWindows( EnumerateWindows, (LPARAM)this );

	return TRUE;
}

BOOL CALLBACK SystemWindowInformation::EnumerateWindows( HWND hwnd, LPARAM lParam )
{
	SystemWindowInformation* _this = (SystemWindowInformation*)lParam;
	WINDOW_INFO wi;

	wi.hWnd = hwnd;
	GetWindowThreadProcessId(hwnd, &wi.ProcessId ) ;

	// Filtering by process ID
	if ( _this->m_processId == ALL_PROCESSES || _this->m_processId == wi.ProcessId )
	{
		GetWindowText( hwnd, wi.Caption, MaxCaptionSize );

		// That is we are looking for
		if ( GetLastError() == 0 )
			_this->m_WindowInfos.AddTail( wi );
	}

	return TRUE;
};

//////////////////////////////////////////////////////////////////////////////////////
