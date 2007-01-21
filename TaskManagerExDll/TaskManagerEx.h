// Hardcoded Information about TaskManagerEx

#ifndef TASKMANAGEREX_H_INCLUDED
#define TASKMANAGEREX_H_INCLUDED

#define TMEX_NAME							_T("Task Manager Extension") // Internal product name

#define TMEX_APP_NAME						_T("Task Manager Extension") // MFC Application name

#define TMEX_REGISTRY_KEY					_T("Microsoft\\Windows NT\\CurrentVersion")
#define TMEX_PROFILE_NAME					_T("TaskManager\\Extension")

#define TMEX_LOADER_EXE						_T("TaskManagerEx.exe")
#define TMEX_INJECT_DLL						_T("TaskManagerExDll.dll")

#define TMEX_EXTENSION_SUBMENU_INDEX		1
#define TMEX_EXTENSION_MENU_FIRST_ITEM		ID_EXTENSION_FINDUSEDFILE
#define TMEX_CPU_MENU_FIRST_ITEM			ID_EXTENSION_CPU00

#define REG_ROOT	HKEY_CURRENT_USER 
#define REG_KEY		_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\TaskManager\\Extension")

#define	APPLICATION_LIST_PID_COLUMN			2 // 0 - Window text, 1 - Status, 2 - our column 'PID'

#endif
