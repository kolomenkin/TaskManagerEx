#ifndef TASKMANAGEREXDLL_H_INCLUDED
#define TASKMANAGEREXDLL_H_INCLUDED
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TASKMANAGEREXDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TASKMANAGEREXDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef TASKMANAGEREXDLL_EXPORTS
#define TASKMANAGEREXDLL_API __declspec(dllexport)
#else
#define TASKMANAGEREXDLL_API __declspec(dllimport)
#endif

extern "C" {
TASKMANAGEREXDLL_API BOOL Initialize();
}

#define ID_PROCESSES_FILES			WM_USER + 1
#define ID_PROCESSES_MODULES		WM_USER + 2
#define ID_PROCESSES_HANDLES		WM_USER + 3
#define ID_PROCESSES_WINDOWS		WM_USER + 4

#define ID_EXTENSION_ABOUT			      WM_USER + 6
#define ID_EXTENSION_FINDUSEDFILE		WM_USER + 7
#define ID_EXTENSION_FINDUSEDMODULE	   WM_USER + 8
#define ID_EXTENSION_CPU00    	      WM_USER + 9
#define ID_EXTENSION_CPU25    	      ID_EXTENSION_CPU00 + 1
#define ID_EXTENSION_CPU50    	      ID_EXTENSION_CPU00 + 2
#define ID_EXTENSION_CPU75    	      ID_EXTENSION_CPU00 + 3
#define ID_EXTENSION_PROCESSICONS      WM_USER + 16
#define ID_EXTENSION_HIDESERVICES      WM_USER + 17

class TaskManagerExDllApp : public CWinApp
{
protected:
   struct StructFindTaskManagerDlgItem
   {	
	   DWORD	itemID;
	   HWND	hwnd;
   };

   struct ProcessesItemData
   {
      DWORD unknown1;
      DWORD unknown2;
      DWORD processId;
      DWORD unknown3;
      DWORD unknown4;
      UCHAR usageCPU;   //in %
   };

   struct ApplicationsItemData
   {
      HWND  hWnd;
   };

protected:
   virtual BOOL InitInstance();
   virtual int ExitInstance();

public:
   BOOL Initialize();
   BOOL Deinitialize();
   
   HWND FindTaskManagerWindow();
   LPCTSTR GetEnglishTaskManagerCaption();
   LPCTSTR GetTaskManagerCaption();
   LPCTSTR GetNewTaskManagerCaption();

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

   void LoadDefaultParameters();
   void SaveDefaultParameters();

   void KillProcess( DWORD );
   
protected:
   DWORD _IsService( DWORD, BOOL& );
   BOOL IsAnotherTaskManagerExDll();

protected:
   static BOOL CALLBACK _EnumChildProc( HWND, LPARAM );
   static LRESULT CALLBACK TaskManagerWndProc( HWND, UINT, WPARAM, LPARAM );
   static LRESULT CALLBACK ProcessesListWndProc( HWND, UINT, WPARAM, LPARAM );
   static LRESULT CALLBACK ProcessesTabWndProc( HWND, UINT, WPARAM, LPARAM );
   static LRESULT CALLBACK ApplicationsListWndProc( HWND, UINT, WPARAM, LPARAM );
   static LRESULT CALLBACK ApplicationsTabWndProc( HWND, UINT, WPARAM, LPARAM );

public:
   HWND              hwndTaskManager;
   HWND              hwndProcessesList;
   HWND              hwndApplicationsList;
   WNDPROC           fnOriginTaskManagerWndProc;
   WNDPROC           fnOriginProcessesList;
   WNDPROC           fnOriginApplicationsList;
   WNDPROC				fnOriginProcessesTab;
   WNDPROC           fnOriginApplicationsTab;

   DWORD             dwWindowsNTMajorVersion;

   HIMAGELIST        hProcessesImageList;
   HICON             hMainIcon;


   CMap<DWORD,DWORD,BOOL,BOOL> mapProcessIsService;

   UCHAR             iProcessUsageAlertLimit;
   COLORREF          clrCPUUsageAlert;
   COLORREF          clrServiceProcess;
   COLORREF          clrDefaultProcess;
   BOOL              bShowProcessesIcons;
   BOOL              bHideServiceProcesses;
};

extern TaskManagerExDllApp theApp;

#endif