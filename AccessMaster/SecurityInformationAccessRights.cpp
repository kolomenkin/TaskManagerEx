//////////////////////////////////////////////////////////////
// File:		// SecurityInformationAccessRights.cpp
// File time:	// 25.04.2005	12:34
// Version: 	// 1.0
/******************************************************************************
Module:  AccessData.h
Notices: Copyright (c) 2000 Jeffrey Richter
Modified: Sergey Kolomenkin, 2005
******************************************************************************/
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#include "stdafx.h"
//#include <windows.h>
#include "SecurityInformation.h"
#include <lmshare.h>
#include <WinSpool.h>

////////////////////////////////////////////////////////////////////////
//                                                                    //
//                             ACCESS MASK                            //
//                                                                    //
////////////////////////////////////////////////////////////////////////

//
//  Define the access mask as a longword sized structure divided up as
//  follows:
//
//       3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//       1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//      +---------------+---------------+-------------------------------+
//      |G|G|G|G|Res'd|A| StandardRights|         SpecificRights        |
//      |R|W|E|A|     |S|               |                               |
//      +-+-------------+---------------+-------------------------------+
//
//      typedef struct _ACCESS_MASK {
//          WORD   SpecificRights;
//          BYTE  StandardRights;
//          BYTE  AccessSystemAcl : 1;
//          BYTE  Reserved : 3;
//          BYTE  GenericAll : 1;
//          BYTE  GenericExecute : 1;
//          BYTE  GenericWrite : 1;
//          BYTE  GenericRead : 1;
//      } ACCESS_MASK;
//      typedef ACCESS_MASK *PACCESS_MASK;
//
//  but to make life simple for programmer's we'll allow them to specify
//  a desired access mask by simply OR'ing together mulitple single rights
//  and treat an access mask as a DWORD.  For example
//
//      DesiredAccess = DELETE | READ_CONTROL
//
//  So we'll declare ACCESS_MASK as DWORD
//
//////////////////////////////////////////////////////////////

GUID CSecurityInformation::m_guidNULL = GUID_NULL;

#define ACCESS_NULL_ENTRY NULL, 0, NULL, 0

// Binary aces
SI_ACCESS CSecurityInformation::m_siAccessBinaryRights[] =
{
	{&m_guidNULL, 0x0001, L"0000000000000001 [Specific 15-0]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x0002, L"0000000000000010 [Specific 15-0]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x0004, L"0000000000000100 [Specific 15-0]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x0008, L"0000000000001000 [Specific 15-0]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x0010, L"0000000000010000 [Specific 15-0]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x0020, L"0000000000100000 [Specific 15-0]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x0040, L"0000000001000000 [Specific 15-0]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x0080, L"0000000010000000 [Specific 15-0]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x0100, L"0000000100000000 [Specific 15-0]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x0200, L"0000001000000000 [Specific 15-0]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x0400, L"0000010000000000 [Specific 15-0]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x0800, L"0000100000000000 [Specific 15-0]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x1000, L"0001000000000000 [Specific 15-0]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x2000, L"0010000000000000 [Specific 15-0]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x4000, L"0100000000000000 [Specific 15-0]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x8000, L"1000000000000000 [Specific 15-0]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x010000, L"DELETE", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x020000, L"READ_CONTROL", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x040000, L"WRITE_DAC", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x080000, L"WRITE_OWNER", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x100000, L"SYNCHRONIZE", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x200000, L"00100000 [Standard 23-16]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x400000, L"01000000 [Standard 23-16]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x800000, L"10000000 [Standard 23-16]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x1000000, L"ACCESS_SYSTEM_SECURITY", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x2000000, L"MAXIMUM_ALLOWED", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x4000000, L"01 [Reserved 27-26]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x8000000, L"10 [Reserved 27-26]", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x10000000, L"GENERIC_ALL", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x20000000, L"GENERIC_EXECUTE", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x40000000, L"GENERIC_WRITE", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{&m_guidNULL, 0x80000000, L"GENERIC_READ", 
		 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
	{ACCESS_NULL_ENTRY}
};

///////////////////////////////////////////////////////////////////////////////


CSecurityInformation::OBJECT_SI_ACCESS CSecurityInformation::m_siAccessAllRights[] =
{ 
	{  // File (0)
		OB_TYPE_FILE,
		SE_FILE_OBJECT,
		{
		  {&m_guidNULL, FILE_ALL_ACCESS, L"FILE_ALL_ACCESS", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_READ_DATA, L"FILE_READ_DATA", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_WRITE_DATA, L"FILE_WRITE_DATA", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_APPEND_DATA, L"FILE_APPEND_DATA", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_READ_EA, L"FILE_READ_EA", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_WRITE_EA, L"FILE_WRITE_EA", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_EXECUTE, L"FILE_EXECUTE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_READ_ATTRIBUTES, L"FILE_READ_ATTRIBUTES", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_WRITE_ATTRIBUTES, L"FILE_WRITE_ATTRIBUTES", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE",
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY",
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, GENERIC_READ, L"GENERIC_READ", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, GENERIC_WRITE, L"GENERIC_WRITE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, GENERIC_EXECUTE, L"GENERIC_EXECUTE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, GENERIC_ALL, L"GENERIC_ALL", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
	  }
	},

	{  // Dir (14)
		OB_TYPE_DIRECTORY,
		SE_FILE_OBJECT,
		{
		  {&m_guidNULL, FILE_ALL_ACCESS, L"FILE_ALL_ACCESS", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_LIST_DIRECTORY, L"FILE_LIST_DIRECTORY", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_ADD_FILE, L"FILE_ADD_FILE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_ADD_SUBDIRECTORY, L"FILE_ADD_SUBDIRECTORY", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_READ_EA, L"FILE_READ_EA", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_WRITE_EA, L"FILE_WRITE_EA", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_TRAVERSE, L"FILE_TRAVERSE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_DELETE_CHILD, L"FILE_DELETE_CHILD", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_READ_ATTRIBUTES, L"FILE_READ_ATTRIBUTES", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_WRITE_ATTRIBUTES, L"FILE_WRITE_ATTRIBUTES", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", 
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, GENERIC_READ, L"GENERIC_READ", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, GENERIC_WRITE, L"GENERIC_WRITE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, GENERIC_EXECUTE, L"GENERIC_EXECUTE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, GENERIC_ALL, L"GENERIC_ALL", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
		}
	},

	{  // Service (29)
		OB_TYPE_SERVICE,
		SE_SERVICE,
		{
		  {&m_guidNULL, SERVICE_ALL_ACCESS, L"SERVICE_ALL_ACCESS", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SERVICE_CHANGE_CONFIG, L"SERVICE_CHANGE_CONFIG", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SERVICE_ENUMERATE_DEPENDENTS, 
				L"SERVICE_ENUMERATE_DEPENDENTS", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SERVICE_INTERROGATE, L"SERVICE_INTERROGATE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SERVICE_PAUSE_CONTINUE, L"SERVICE_PAUSE_CONTINUE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SERVICE_QUERY_CONFIG, L"SERVICE_QUERY_CONFIG", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SERVICE_QUERY_STATUS, L"SERVICE_QUERY_STATUS", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SERVICE_START, L"SERVICE_START", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SERVICE_STOP, L"SERVICE_STOP", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SERVICE_USER_DEFINED_CONTROL, 
				L"SERVICE_USER_DEFINED_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", 
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
		}
	},

	{  // Printer (44)
		OB_TYPE_PRINTER,
		SE_PRINTER,
		{
		  {&m_guidNULL, SERVER_ACCESS_ADMINISTER, L"SERVER_ACCESS_ADMINISTER", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SERVER_ACCESS_ENUMERATE, L"SERVER_ACCESS_ENUMERATE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, PRINTER_ACCESS_ADMINISTER, L"PRINTER_ACCESS_ADMINISTER", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, PRINTER_ACCESS_USE, L"PRINTER_ACCESS_USE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, JOB_ACCESS_ADMINISTER, L"JOB_ACCESS_ADMINISTER", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", 
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
		}
	},

	{  // Registry (55)
		OB_TYPE_KEY,
		SE_REGISTRY_KEY,
		{
		  {&m_guidNULL, KEY_ALL_ACCESS, L"KEY_ALL_ACCESS", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, KEY_QUERY_VALUE, L"KEY_QUERY_VALUE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, KEY_SET_VALUE, L"KEY_SET_VALUE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, KEY_CREATE_SUB_KEY, L"KEY_CREATE_SUB_KEY", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, KEY_ENUMERATE_SUB_KEYS, L"KEY_ENUMERATE_SUB_KEYS", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, KEY_NOTIFY, L"KEY_NOTIFY", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, KEY_CREATE_LINK, L"KEY_CREATE_LINK", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE", 
//			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC}, 
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", 
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
		}
	},

	{  // Share (68)
		OB_TYPE_SHARE,
		SE_LMSHARE,
		{
		  {&m_guidNULL, PERM_FILE_READ, L"PERM_FILE_READ", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, PERM_FILE_WRITE, L"PERM_FILE_WRITE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, PERM_FILE_CREATE, L"PERM_FILE_CREATE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", 
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
		}
	},

	{  // Process (74)
		OB_TYPE_PROCESS,
		SE_KERNEL_OBJECT,
		{
		  {&m_guidNULL, PROCESS_ALL_ACCESS, L"PROCESS_ALL_ACCESS", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, PROCESS_TERMINATE, L"PROCESS_TERMINATE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, PROCESS_CREATE_THREAD, L"PROCESS_CREATE_THREAD", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, PROCESS_SET_SESSIONID, L"PROCESS_SET_SESSIONID", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, PROCESS_VM_OPERATION, L"PROCESS_VM_OPERATION", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, PROCESS_VM_READ, L"PROCESS_VM_READ", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, PROCESS_VM_WRITE, L"PROCESS_VM_WRITE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, PROCESS_DUP_HANDLE, L"PROCESS_DUP_HANDLE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, PROCESS_CREATE_PROCESS, L"PROCESS_CREATE_PROCESS", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, PROCESS_SET_QUOTA, L"PROCESS_SET_QUOTA", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, PROCESS_SET_INFORMATION, L"PROCESS_SET_INFORMATION", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, PROCESS_QUERY_INFORMATION, L"PROCESS_QUERY_INFORMATION", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", 
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
		}
	},

	{  // Thread (91)
		OB_TYPE_THREAD,
		SE_KERNEL_OBJECT,
		{
		  {&m_guidNULL, THREAD_ALL_ACCESS, L"THREAD_ALL_ACCESS", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, THREAD_TERMINATE, L"THREAD_TERMINATE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, THREAD_SUSPEND_RESUME, L"THREAD_SUSPEND_RESUME", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, THREAD_GET_CONTEXT, L"THREAD_GET_CONTEXT", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, THREAD_SET_CONTEXT, L"THREAD_SET_CONTEXT", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, THREAD_SET_INFORMATION, L"THREAD_SET_INFORMATION", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, THREAD_QUERY_INFORMATION, L"THREAD_QUERY_INFORMATION", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, THREAD_SET_THREAD_TOKEN, L"THREAD_SET_THREAD_TOKEN", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, THREAD_IMPERSONATE, L"THREAD_IMPERSONATE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, THREAD_DIRECT_IMPERSONATION, 
				L"THREAD_DIRECT_IMPERSONATION", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", 
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
		}
	},

	{  // Job (107)
		OB_TYPE_JOB,
		SE_KERNEL_OBJECT,
		{
		  {&m_guidNULL, JOB_OBJECT_ALL_ACCESS, L"JOB_OBJECT_ALL_ACCESS", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, JOB_OBJECT_ASSIGN_PROCESS, L"JOB_OBJECT_ASSIGN_PROCESS", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, JOB_OBJECT_SET_ATTRIBUTES, L"JOB_OBJECT_SET_ATTRIBUTES", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, JOB_OBJECT_QUERY, L"JOB_OBJECT_QUERY", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, JOB_OBJECT_TERMINATE, L"JOB_OBJECT_TERMINATE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, JOB_OBJECT_SET_SECURITY_ATTRIBUTES, 
				L"JOB_OBJECT_SET_SECURITY_ATTRIBUTES", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", 
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
		}
	},

	{  // Semaphore
		OB_TYPE_SEMAPHORE,
		SE_KERNEL_OBJECT,
		{
		  {&m_guidNULL, SEMAPHORE_ALL_ACCESS, L"SEMAPHORE_ALL_ACCESS", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SEMAPHORE_MODIFY_STATE, L"SEMAPHORE_MODIFY_STATE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", 
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
		}
	},

	{  // Event
		OB_TYPE_EVENT,
		SE_KERNEL_OBJECT,
		{
		  {&m_guidNULL, EVENT_ALL_ACCESS, L"EVENT_ALL_ACCESS", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, EVENT_MODIFY_STATE, L"EVENT_MODIFY_STATE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", 
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, GENERIC_READ, L"GENERIC_READ", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, GENERIC_WRITE, L"GENERIC_WRITE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, GENERIC_EXECUTE, L"GENERIC_EXECUTE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, GENERIC_ALL, L"GENERIC_ALL", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
		}
	},

	{  // Mutex
		OB_TYPE_MUTANT,
		SE_KERNEL_OBJECT,
		{
		  {&m_guidNULL, MUTEX_ALL_ACCESS, L"MUTEX_ALL_ACCESS", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, MUTEX_MODIFY_STATE, L"MUTEX_MODIFY_STATE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", 
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
		}
	},

	{  // Mapping
		OB_TYPE_SECTION,
		SE_KERNEL_OBJECT,
		{
		  {&m_guidNULL, FILE_MAP_COPY, L"FILE_MAP_COPY", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_MAP_WRITE, L"FILE_MAP_WRITE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_MAP_READ, L"FILE_MAP_READ", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_MAP_ALL_ACCESS, L"FILE_MAP_ALL_ACCESS", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SECTION_EXTEND_SIZE, L"SECTION_EXTEND_SIZE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", 
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
		}
	},

	{  // Timer
		OB_TYPE_TIMER,
		SE_KERNEL_OBJECT,
		{
		  {&m_guidNULL, TIMER_ALL_ACCESS, L"TIMER_ALL_ACCESS", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, TIMER_QUERY_STATE, L"TIMER_QUERY_STATE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, TIMER_MODIFY_STATE, L"TIMER_MODIFY_STATE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", 
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
		}
	},

	{  // Token
		OB_TYPE_TOKEN,
		SE_KERNEL_OBJECT,
		{
		  {&m_guidNULL, TOKEN_ALL_ACCESS, L"TOKEN_ALL_ACCESS", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, TOKEN_ASSIGN_PRIMARY, L"TOKEN_ASSIGN_PRIMARY", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, TOKEN_DUPLICATE, L"TOKEN_DUPLICATE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, TOKEN_IMPERSONATE, L"TOKEN_IMPERSONATE", 
				 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, TOKEN_QUERY, L"TOKEN_QUERY", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, TOKEN_QUERY_SOURCE, L"TOKEN_QUERY_SOURCE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, TOKEN_ADJUST_PRIVILEGES, L"TOKEN_ADJUST_PRIVILEGES", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, TOKEN_ADJUST_GROUPS, L"TOKEN_ADJUST_GROUPS", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, TOKEN_ADJUST_DEFAULT, L"TOKEN_ADJUST_DEFAULT", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, TOKEN_ADJUST_SESSIONID, L"TOKEN_ADJUST_SESSIONID", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", 
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
		}
	},

	{  // Namedpipe
		OB_TYPE_NAMEDPIPE,
		SE_KERNEL_OBJECT,
		{
		  {&m_guidNULL, FILE_ALL_ACCESS, L"FILE_ALL_ACCESS", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_READ_DATA, L"FILE_READ_DATA", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_WRITE_DATA, L"FILE_WRITE_DATA", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_CREATE_PIPE_INSTANCE, L"FILE_CREATE_PIPE_INSTANCE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_READ_ATTRIBUTES, L"FILE_READ_ATTRIBUTES", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_WRITE_ATTRIBUTES, L"FILE_WRITE_ATTRIBUTES", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", 
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
		}
	},

	{  // Anonpipe
		OB_TYPE_ANONPIPE,
		SE_KERNEL_OBJECT,
		{
		  {&m_guidNULL, FILE_ALL_ACCESS, L"FILE_ALL_ACCESS", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_READ_DATA, L"FILE_READ_DATA", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_WRITE_DATA, L"FILE_WRITE_DATA", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_CREATE_PIPE_INSTANCE, L"FILE_CREATE_PIPE_INSTANCE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_READ_ATTRIBUTES, L"FILE_READ_ATTRIBUTES", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, FILE_WRITE_ATTRIBUTES, L"FILE_WRITE_ATTRIBUTES", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", 
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
		}
	},

	{  // Windowstation
		OB_TYPE_WINDOW_STATION,
		SE_WINDOW_OBJECT,
		{
		  {&m_guidNULL, WINSTA_ACCESSCLIPBOARD, L"WINSTA_ACCESSCLIPBOARD", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WINSTA_ACCESSGLOBALATOMS, L"WINSTA_ACCESSGLOBALATOMS", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WINSTA_CREATEDESKTOP, L"WINSTA_CREATEDESKTOP", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WINSTA_ENUMDESKTOPS, L"WINSTA_ENUMDESKTOPS", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WINSTA_ENUMERATE, L"WINSTA_ENUMERATE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WINSTA_EXITWINDOWS, L"WINSTA_EXITWINDOWS", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WINSTA_READATTRIBUTES, L"WINSTA_READATTRIBUTES", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WINSTA_READSCREEN, L"WINSTA_READSCREEN", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WINSTA_WRITEATTRIBUTES, L"WINSTA_WRITEATTRIBUTES", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE", 
//			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", 
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, GENERIC_READ, L"GENERIC_READ", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, GENERIC_WRITE, L"GENERIC_WRITE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, GENERIC_EXECUTE, L"GENERIC_EXECUTE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, GENERIC_ALL, L"GENERIC_ALL", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
		}
	},

	{  // Desktop
		OB_TYPE_DESKTOP,
		SE_WINDOW_OBJECT,
		{
		  {&m_guidNULL, DESKTOP_CREATEMENU, L"DESKTOP_CREATEMENU", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DESKTOP_CREATEWINDOW, L"DESKTOP_CREATEWINDOW", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DESKTOP_ENUMERATE, L"DESKTOP_ENUMERATE", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DESKTOP_HOOKCONTROL, L"DESKTOP_HOOKCONTROL", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DESKTOP_JOURNALPLAYBACK, L"DESKTOP_JOURNALPLAYBACK", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DESKTOP_JOURNALRECORD, L"DESKTOP_JOURNALRECORD", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DESKTOP_READOBJECTS, L"DESKTOP_READOBJECTS", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DESKTOP_SWITCHDESKTOP, L"DESKTOP_SWITCHDESKTOP", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DESKTOP_WRITEOBJECTS, L"DESKTOP_WRITEOBJECTS", 
				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, READ_CONTROL, L"READ_CONTROL", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_DAC, L"WRITE_DAC", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, WRITE_OWNER, L"WRITE_OWNER", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {&m_guidNULL, DELETE, L"DELETE", 
			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, SYNCHRONIZE, L"SYNCHRONIZE", 
//			 SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
//		  {&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", 
//				SI_ACCESS_GENERAL|SI_ACCESS_SPECIFIC},
		  {ACCESS_NULL_ENTRY}
		}
	},

	{  // Empty
		OB_TYPE_UNKNOWN,
		SE_UNKNOWN_OBJECT_TYPE,
		{
		 {ACCESS_NULL_ENTRY}
		}
	}
};

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
