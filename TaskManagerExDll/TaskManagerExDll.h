#ifndef TASKMANAGEREXDLL_H_INCLUDED
#define TASKMANAGEREXDLL_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////

#include "TaskManagerExDllExport.h"
#include "SystemInfo.h"
#include "Options.h"

#define SHELL_OPERATION_FILE_PROPERTIES		_T("properties")
#define SHELL_OPERATION_VIEW_DEPENDENCY		_T("View Dependencies")

BOOL MyShellExecute( HWND hwnd, LPCTSTR szOperation, LPCTSTR szFileName );

BOOL IsDependencyWalkerInstalled();
BOOL FreeSearchInString( LPCTSTR lpszString, LPCTSTR lpszSubString );

BOOL GetProcessExecutableName( DWORD dwPID, LPTSTR szName, int cbSize );
CString GetProcessName( DWORD dwPID );

////////////////////////////////////////////////////////////////////////////////

class TaskManagerExDllApp : public CWinApp, IPsapi
{
public:
	TaskManagerExDllApp();
	~TaskManagerExDllApp();

protected:

	struct StructFindTaskManagerDlgItem
	{	
		DWORD	itemID;
		HWND	hwnd;
	};

	struct ProcessesItemData
	{
		DWORD	unkUpdateCounter; // 0..
		DWORD	unknown1;		//0
		DWORD	processId;		//!
		PWSTR	UserName;		//!   // Win XP:
		DWORD	unknown4;		//0
		UCHAR	usageCPU;		// %
		// Win XP:
		UCHAR	usageCPU_2;		// %, equal to usageCPU
		UCHAR	unknown6_3;		//0
		UCHAR	unknown6_4;		//0
		DWORD	CpuTime1;		//! FILETIME
		DWORD	unknown8;		//0/1
		DWORD	CpuTime2;		//! FILETIME, equal to CpuTime1
		DWORD	unknown10;		//0/1
		DWORD	MemUsage;		//! kb
		DWORD	_MemDelta;		//0/300 kb
		DWORD	PageFaults;		//!
		DWORD	PFDelta;		//!
		DWORD	VMSize;			//! kb
		DWORD	PagedPool;		//! kb
		DWORD	NPPool;			//! kb
		DWORD	BasePri;		//!
		DWORD	Handles;		//!
		DWORD	Threads;		//!
		DWORD	GdiObjects;		//! //
		DWORD	UserObjects;	//! // GDI <-> User
		DWORD	IoReads;		//!
		DWORD	unknown24;		//0
		DWORD	IoWrites;		//!
		DWORD	unknown26;		//0
		DWORD	IoOther;		//!
		DWORD	unknown28;		//0
		DWORD	IoReadBytes;	//!
		DWORD	unknown30;		//0
		DWORD	IoWriteBytes;	//!
		DWORD	unknown32;		//0
		DWORD	IoOtherBytes;	//!
		DWORD	unknown34;		//0
		DWORD	unknown35;		//big
		DWORD	unknown36;		//0
		DWORD	unknown37;		//0
		DWORD	unknown38;		//2
		DWORD	PeakMemUsage;	//! kb
		DWORD	unknown40;
		DWORD	unknown41;
		DWORD	unknown42;
		DWORD	unknown43;
		DWORD	unknown44;
		DWORD	unknown45;
		DWORD	unknown46;
		DWORD	unknown47;
		DWORD	unknown48;
		DWORD	unknown49;
		DWORD	unknown50;
		// etc...
	};

	struct ApplicationsItemData
	{
		HWND		hWnd;
		// Win XP:
		LPWSTR	Caption;
		LPWSTR	WindowStation;
		LPWSTR	Desktop;
		DWORD		Unknown4; // 0
		DWORD		Unknown5; // 0
		DWORD		Unknown6; // 0x33
		DWORD		Unknown7; // 0
		DWORD		index;    // 0 .. item_count-1
		DWORD		Unknown9;
		DWORD		Unknown10;
		DWORD		Unknown11;
		DWORD		Unknown12;
		DWORD		Unknown13;
		// etc...
	};

protected:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

public:
	BOOL Initialize();
	BOOL Deinitialize();

	LPCTSTR GetNewTaskManagerCaption();
	bool IsTaskManagerWindowTopMost();

	void UpdateMainMenu( HMENU );
	BOOL IsProcessIdValid( DWORD );

	HWND FindTaskManagerDlgItem( DWORD );
	void RefreshProcessList();

	ProcessesItemData* GetSelectedProcessData();
	ProcessesItemData* GetProcessData( int );
	ApplicationsItemData* GetApplicationsData( int nItem );

	BOOL IsProcessService( DWORD );

	DWORD GetProcessesListDlgID();
	DWORD GetApplicationsListDlgID();

	int FindProcessImageIndex( DWORD );

	void SetProcessesIcons( BOOL );

	void UpdateApplicationsListView();

	void KillProcess( DWORD );
   
protected:
	DWORD _IsService( DWORD, BOOL& );
	BOOL IsAnotherTaskManagerExDll();

	CImageList MicroImageList;

protected:
	static BOOL CALLBACK _EnumChildProc( HWND, LPARAM );
	static LRESULT CALLBACK TaskManagerWndProc( HWND, UINT, WPARAM, LPARAM );
	static LRESULT CALLBACK ProcessesListWndProc( HWND, UINT, WPARAM, LPARAM );
	static LRESULT CALLBACK ProcessesTabWndProc( HWND, UINT, WPARAM, LPARAM );
	static LRESULT CALLBACK ApplicationsListWndProc( HWND, UINT, WPARAM, LPARAM );
	static LRESULT CALLBACK ApplicationsTabWndProc( HWND, UINT, WPARAM, LPARAM );

	// This window has original caption and class to be found by Taskman.exe.
	// It is always hidden.
	HWND			m_hwndFakeWindow;
	WNDPROC			fnOriginFakeWndProc;
	BOOL CreateFakeWindow();
	BOOL InitFakeWindow(HINSTANCE hinstance);
	static LRESULT CALLBACK FakeWndProc( HWND, UINT, WPARAM, LPARAM );

public:
	HWND			hwndTaskManager;
	HWND			hwndProcessesList;
	HWND			hwndApplicationsList;
	WNDPROC			fnOriginTaskManagerWndProc;
	WNDPROC			fnOriginProcessesList;
	WNDPROC			fnOriginApplicationsList;
	WNDPROC			fnOriginProcessesTab;
	WNDPROC			fnOriginApplicationsTab;

	DWORD			dwWindowsNTMajorVersion;

	HIMAGELIST		hProcessesImageList;
	HICON			hMainIconSmall;
	HICON			hMainIconBig;


	CMap<DWORD,DWORD,BOOL,BOOL> mapProcessIsService;

	COptions		m_options;
	
public:
	void OnProperties();
	void ShowTipAtStartup(void);
	void ShowTipOfTheDay(void);
};

extern TaskManagerExDllApp theApp;

#endif //ifndef TASKMANAGEREXDLL_H_INCLUDED
