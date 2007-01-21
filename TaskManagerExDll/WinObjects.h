//////////////////////////////////////////////////////////////
// File:		// WinObjects.h
// File time:	// 26.04.2005	23:26
// Version: 	// 1.0
// Author:		// Sergey Kolomenkin <reinhard@tut.by>
// Copyright:	// (c) Sergey Kolomenkin, Minsk, 2005
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#ifdef WIN_OBJECT_IMPLEMENT
#undef _WINOBJECTS_H_UID00000024CBF5C551
#endif

#ifndef _WINOBJECTS_H_UID00000024CBF5C551
#define _WINOBJECTS_H_UID00000024CBF5C551

	//////////////////////////////////////////////////////////////////////////

#ifndef WIN_OBJECT_IMPLEMENT

	#include <accctrl.h>

	enum OB_TYPE_ENUM{
	#define DEFINE_OBJ_TYPE( id, sys_name, screen_name, securType, IsChild, IsContainer ) id,

#else

	ObjTypeInf g_ObjectTypes[] = 
	{
	#undef  DEFINE_OBJ_TYPE
	#define DEFINE_OBJ_TYPE( id, sys_name, screen_name, securType, IsChild, IsContainer ) { id, sys_name, screen_name, securType, IsChild, IsContainer },
#endif

	//////////////////////////////////////////////////////////////////////////

	DEFINE_OBJ_TYPE( OB_TYPE_UNKNOWN,		_T(""),				_T("Unknown"),			SE_KERNEL_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_WINDOW_STATION,_T("WindowStation"),_T("Window Station"),	SE_WINDOW_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_DESKTOP,		_T("Desktop"),		_T("Desktop"),			SE_WINDOW_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_PROCESS,		_T("Process"),		_T("Process"),			SE_KERNEL_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_THREAD,		_T("Thread"),		_T("Thread"),			SE_KERNEL_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_FILE,			_T("File"),			_T("File"),				SE_FILE_OBJECT,		TRUE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_SECTION,		_T("Section"),		_T("Section"),			SE_KERNEL_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_TOKEN,			_T("Token"),		_T("Access Token"),		SE_KERNEL_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_DIRECTORY,		_T("Directory"),	_T("Directory"),		SE_FILE_OBJECT,		TRUE,	TRUE )
	DEFINE_OBJ_TYPE( OB_TYPE_SYMBOLIC_LINK,	_T("SymbolicLink"),	_T("Symbolic Link"),	SE_KERNEL_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_KEY,			_T("Key"),			_T("Registry Key"),		SE_REGISTRY_KEY,	TRUE,	TRUE )
	DEFINE_OBJ_TYPE( OB_TYPE_PORT,			_T("Port"),			_T("Port"),				SE_KERNEL_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_EVENT,			_T("Event"),		_T("Event"),			SE_KERNEL_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_EVENT_PAIR,	_T("EventPair"),	_T("Event Pair"),		SE_KERNEL_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_MUTANT,		_T("Mutant"),		_T("Mutant"),			SE_KERNEL_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_SEMAPHORE,		_T("Semaphore"),	_T("Semaphore"),		SE_KERNEL_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_TIMER,			_T("Timer"),		_T("Timer"),			SE_KERNEL_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_WAITABLE_PORT,	_T("WaitablePort"),	_T("Waitable Port"),	SE_KERNEL_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_IO_COMPLETION,	_T("IoCompletion"),	_T("IoCompletion"),		SE_KERNEL_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_KEYED_EVENT,	_T("KeyedEvent"),	_T("Keyed Event"),		SE_KERNEL_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_PROFILE,		_T("Profile"),		_T("Profile"),			SE_KERNEL_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_WMI_GUID,		_T("WmiGuid"),		_T("Wmi Guid"),			SE_WMIGUID_OBJECT,	FALSE,	FALSE )
	DEFINE_OBJ_TYPE( OB_TYPE_JOB,			_T("Job"),			_T("Job"),				SE_KERNEL_OBJECT,	FALSE,	FALSE )

	DEFINE_OBJ_TYPE( OB_TYPE_CONTROLLER,	_T("Controller"),	_T("Controller"),		SE_KERNEL_OBJECT,	FALSE,	FALSE ) // I've never seen such object
	DEFINE_OBJ_TYPE( OB_TYPE_DEVICE,		_T("Device"),		_T("Device"),			SE_KERNEL_OBJECT,	FALSE,	FALSE ) // I've never seen such object
	DEFINE_OBJ_TYPE( OB_TYPE_DRIVER,		_T("Driver"),		_T("Driver"),			SE_KERNEL_OBJECT,	FALSE,	FALSE ) // I've never seen such object
	DEFINE_OBJ_TYPE( OB_TYPE_DEBUG_OBJECT,	_T("DebugObject"),	_T("Debug Object"),		SE_KERNEL_OBJECT,	FALSE,	FALSE ) // I've never seen such object
	DEFINE_OBJ_TYPE( OB_TYPE_ADAPTER,		_T("Adapter"),		_T("Adapter"),			SE_KERNEL_OBJECT,	FALSE,	FALSE ) // I've never seen such object
	DEFINE_OBJ_TYPE( OB_TYPE_CALLBACK,		_T("Callback"),		_T("Callback"),			SE_KERNEL_OBJECT,	FALSE,	FALSE ) // I've never seen such object

	DEFINE_OBJ_TYPE( OB_TYPE_NAMEDPIPE,		_T("#"),			_T("Named Pipe"),		SE_KERNEL_OBJECT,	FALSE,	FALSE ) // OB_TYPE_FILE: "\Device\NamepPipe\"
	DEFINE_OBJ_TYPE( OB_TYPE_ANONPIPE,		_T("#"),			_T("Anonymous Pipe"),	SE_KERNEL_OBJECT,	FALSE,	FALSE ) // OB_TYPE_FILE: "\Device\NamepPipe\Win32Pipes."
	DEFINE_OBJ_TYPE( OB_TYPE_PRINTER,		_T("#"),			_T("Printer"),			SE_PRINTER,			FALSE,	FALSE ) // localname or "\\machinename\printername"
	DEFINE_OBJ_TYPE( OB_TYPE_SERVICE,		_T("#"),			_T("Service"),			SE_SERVICE,			FALSE,	FALSE ) // localname or "\\machinename\servicename"
	DEFINE_OBJ_TYPE( OB_TYPE_SHARE,			_T("#"),			_T("Share"),			SE_LMSHARE,			FALSE,	FALSE ) // localname or "\\machinename\sharename"

	// _T("#") means that this object has no special name or is not a named kernel object

	//////////////////////////////////////////////////////////////////////////
	
#ifndef WIN_OBJECT_IMPLEMENT
		OB_TYPES_COUNT
	};

//	#define OB_TYPE_REGISTRY		OB_TYPE_KEY
//	#define OB_TYPE_DIR				OB_TYPE_DIRECTORY
//	#define OB_TYPE_MAPPING			OB_TYPE_SECTION
//	#define OB_TYPE_MUTEX			OB_TYPE_MUTANT

	typedef struct _ObjTypeInf {
		OB_TYPE_ENUM	m_objInternalType;
		LPCTSTR			m_szName;
		LPCTSTR			m_szScreenName;
		SE_OBJECT_TYPE	m_objSecurType;
		BOOL			m_bIsChild;
		BOOL			m_bIsContainer;
	} ObjTypeInf;

	BOOL DecodeWinObjectType( IN LPCTSTR typeToken, OUT OB_TYPE_ENUM& type );
	BOOL GetWinObjectTypeInfo( IN OB_TYPE_ENUM& type, OUT ObjTypeInf& type_info );

#else

	};

#endif

	//////////////////////////////////////////////////////////////////////////

#endif //ifndef _WINOBJECTS_H_UID00000024CBF5C551
