// Export information about TaskManagerEx.dll

////////////////////////////////////////////////////////////////////////////////
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TASKMANAGEREXDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TASKMANAGEREXDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef TASKMANAGEREXDLL_EXPORTS
//#define TASKMANAGEREXDLL_API __declspec(dllexport)
#define TASKMANAGEREXDLL_API	// no need to specify dllexport once we specified the same functions in .def file (and are using extern "C")
#else
#define TASKMANAGEREXDLL_API __declspec(dllimport)
#endif

#ifdef TASKMANAGEREXDLL_EXPORTS
#	ifdef _DEBUG
#		define TASKMANAGEREXDLL_DEBUG_API __declspec(dllexport)
#	else
#		define TASKMANAGEREXDLL_DEBUG_API
#	endif
#else
#	ifdef TASKMANEX_EXE
#		define TASKMANAGEREXDLL_DEBUG_API
#	else
#		define TASKMANAGEREXDLL_DEBUG_API __declspec(dllimport)
#	endif
#endif

extern "C"
{
	TASKMANAGEREXDLL_API BOOL WINAPI Initialize();
	TASKMANAGEREXDLL_API BOOL WINAPI TestProc();
}

TASKMANAGEREXDLL_DEBUG_API void EnableDebugPriv( void );

////////////////////////////////////////////////////////////////////////////////

#define	TASKMANAGEREX_WINDOW_LONG_USER_MAGIC_VALUE		275634

////////////////////////////////////////////////////////////////////////////////

#define	WM_TASKMANAGER_BASE				( WM_APP + 500 )

#define	WM_TASKMANAGER_FLAG				( WM_TASKMANAGER_BASE + 0 )
#define	WM_TASKMANAGER_FLAG_RET_VAL	0xaf2891fa

#define	WM_TASKMANAGER_TIP_OF_THE_DAY	( WM_TASKMANAGER_BASE + 1 )
#define	WM_TASKMANAGER_CREATE_FAKIE_WND	( WM_TASKMANAGER_BASE + 2 )

////////////////////////////////////////////////////////////////////////////////

#define	ID_TASKMANAGER_EX_BASE			30500

#define ID_PROCESSES_FILES				ID_TASKMANAGER_EX_BASE + 1
#define ID_PROCESSES_MODULES			ID_TASKMANAGER_EX_BASE + 2
#define ID_PROCESSES_HANDLES			ID_TASKMANAGER_EX_BASE + 3
#define ID_PROCESSES_WINDOWS			ID_TASKMANAGER_EX_BASE + 4
#define ID_PROCESSES_THREADS			ID_TASKMANAGER_EX_BASE + 5
#define ID_PROCESSES_MEMORY				ID_TASKMANAGER_EX_BASE + 6
#define ID_PROCESSES_INFO				ID_TASKMANAGER_EX_BASE + 7
#define ID_PROCESSES_EXE_PROPERTIES		ID_TASKMANAGER_EX_BASE + 8
#define ID_PROCESSES_EXE_DEPENDENCY		ID_TASKMANAGER_EX_BASE + 9
#define ID_PROCESSES_PROCESS_SECURITY	ID_TASKMANAGER_EX_BASE + 10

#define ID_EXTENSION_ABOUT				ID_TASKMANAGER_EX_BASE + 20
#define ID_EXTENSION_FINDUSEDFILE		ID_TASKMANAGER_EX_BASE + 21
#define ID_EXTENSION_FINDUSEDMODULE		ID_TASKMANAGER_EX_BASE + 22
#define ID_EXTENSION_PROCESSICONS		ID_TASKMANAGER_EX_BASE + 23
#define ID_EXTENSION_HIDESERVICES		ID_TASKMANAGER_EX_BASE + 24
#define ID_EXTENSION_SHOW_DRIVERS		ID_TASKMANAGER_EX_BASE + 25
#define ID_EXTENSION_ALL_HANDLES		ID_TASKMANAGER_EX_BASE + 26
#define ID_EXTENSION_TIP_OF_THE_DAY     ID_TASKMANAGER_EX_BASE + 27

#define ID_EXTENSION_CPU00				ID_TASKMANAGER_EX_BASE + 50
#define ID_EXTENSION_CPU25				ID_EXTENSION_CPU00 + 1
#define ID_EXTENSION_CPU50				ID_EXTENSION_CPU00 + 2
#define ID_EXTENSION_CPU75				ID_EXTENSION_CPU00 + 3

////////////////////////////////////////////////////////////////////////////////
