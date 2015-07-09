// Hardcoded Information about Windows Task Manager (taskmgr.exe)

#pragma once

#define TASKMGR_END_PROCESS_CMD				40028	// first element of Task Manager default process popup menu: "End Process"
#define TASKMGR_OPEN_FILE_LOCATION_CMD		50127	// first element of Task Manager default process popup menu (starting from Vista): "Open File Location"
#define TASKMGR_ALWAYS_ON_TOP_CMD			40006

#define TASKMGR_LIBRARY						_T("taskmgr.exe")
#define TASKMGR_IDS_WINDOW_CAPTION			10003

#define TASKMGR_DEFAULT_NT4_WINDOW_CAPTION	_T("Windows NT Task Manager")
#define TASKMGR_DEFAULT_WINDOW_CAPTION		_T("Windows Task Manager")
#define TASKMGR_DEFAULT_WINDOW_CLASS		( (LPCTSTR)32770 )

#define WM_TASKMAN_SET_FOREGROUND	( WM_USER + 0x0B )	// this message is sent to popup previously started taskmgr window

#define TASKMGR_PROCESS_LIST_DLG_ID			0x3F1	// 1009
#define TASKMGR_APPLICATIONS_LIST_DLG_ID	0x41D	// 1053

//#define TASKMGR_APPLICATIONS_LIST_ITEM_2_IMAGE( iItem )		(iItem+1) // bad

HWND FindTaskManagerWindow();
LPCTSTR GetTaskManagerCaption();
