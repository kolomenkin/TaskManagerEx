# Microsoft Developer Studio Project File - Name="TaskManagerExDll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=TaskManagerExDll - Win32 Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TaskManagerExDll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TaskManagerExDll.mak" CFG="TaskManagerExDll - Win32 Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TaskManagerExDll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TaskManagerExDll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TaskManagerExDll - Win32 Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TaskManagerExDll - Win32 Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/TaskManagerExDll", HAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TaskManagerExDll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin\Release"
# PROP Intermediate_Dir "..\Intermediate\Release\TaskManagerExDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TASKMANAGEREXDLL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W4 /GX /O2 /D "NDEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "TASKMANAGEREXDLL_EXPORTS" /D "_WINDLL" /D "TASKMANEX_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "TaskManagerExDll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin\Debug"
# PROP Intermediate_Dir "..\Intermediate\Debug\TaskManagerExDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TASKMANAGEREXDLL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "TASKMANAGEREXDLL_EXPORTS" /D "_WINDLL" /D "TASKMANEX_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /dll /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "TaskManagerExDll - Win32 Release Unicode"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TaskManagerExDll___Win32_Release_Unicode"
# PROP BASE Intermediate_Dir "TaskManagerExDll___Win32_Release_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin\Release Unicode"
# PROP Intermediate_Dir "..\Intermediate\Release Unicode\TaskManagerExDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TASKMANAGEREXDLL_EXPORTS" /D "WINNT" /D "_WINDLL" /YX"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W4 /GX /O2 /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "TASKMANAGEREXDLL_EXPORTS" /D "_WINDLL" /D "TASKMANEX_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib /nologo /dll /machine:I386 /out:"../bin/TaskManagerEx.dll"
# ADD LINK32 /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "TaskManagerExDll - Win32 Debug Unicode"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TaskManagerExDll___Win32_Debug_Unicode"
# PROP BASE Intermediate_Dir "TaskManagerExDll___Win32_Debug_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin\Debug Unicode"
# PROP Intermediate_Dir "..\Intermediate\Debug Unicode\TaskManagerExDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TASKMANAGEREXDLL_EXPORTS" /D "WINNT" /D "_WINDLL" /YX"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "TASKMANAGEREXDLL_EXPORTS" /D "_WINDLL" /D "TASKMANEX_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib /nologo /dll /debug /machine:I386 /out:"../bin/Debug/TaskManagerEx.dll" /pdbtype:sept
# ADD LINK32 /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "TaskManagerExDll - Win32 Release"
# Name "TaskManagerExDll - Win32 Debug"
# Name "TaskManagerExDll - Win32 Release Unicode"
# Name "TaskManagerExDll - Win32 Debug Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutExtension.cpp
# End Source File
# Begin Source File

SOURCE=.\ApiHooks.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorBox.cpp
# End Source File
# Begin Source File

SOURCE=.\DllVersion.cpp
# End Source File
# Begin Source File

SOURCE=.\FindTextDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HookImportFunction.cpp
# End Source File
# Begin Source File

SOURCE=.\InformationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadDll.cpp
# End Source File
# Begin Source File

SOURCE=.\Localization.cpp
# End Source File
# Begin Source File

SOURCE=.\Options.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsPropertyPages.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SystemInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\SystemInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SystemInfoListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SystemInfoUI.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskManagerExDll.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskManagerExDll.def
# End Source File
# Begin Source File

SOURCE=.\Taskmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\TipDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VMQuery.cpp
# End Source File
# Begin Source File

SOURCE=.\WindowsCore.cpp
# End Source File
# Begin Source File

SOURCE=.\WinObjects.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AboutExtension.h
# End Source File
# Begin Source File

SOURCE=.\ApiHooks.h
# End Source File
# Begin Source File

SOURCE=.\ColorBox.h
# End Source File
# Begin Source File

SOURCE=.\DllVersion.h
# End Source File
# Begin Source File

SOURCE=.\FindTextDlg.h
# End Source File
# Begin Source File

SOURCE=.\HookImportFunction.h
# End Source File
# Begin Source File

SOURCE=.\InformationDlg.h
# End Source File
# Begin Source File

SOURCE=.\LoadDll.h
# End Source File
# Begin Source File

SOURCE=.\Localization.h
# End Source File
# Begin Source File

SOURCE=.\Options.h
# End Source File
# Begin Source File

SOURCE=.\OptionsPropertyPages.h
# End Source File
# Begin Source File

SOURCE=.\OptionsPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SystemInfo.h
# End Source File
# Begin Source File

SOURCE=.\SystemInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\SystemInfoListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SystemInfoUI.h
# End Source File
# Begin Source File

SOURCE=.\TaskManagerEx.h
# End Source File
# Begin Source File

SOURCE=.\TaskManagerExDll.h
# End Source File
# Begin Source File

SOURCE=.\TaskManagerExDllExport.h
# End Source File
# Begin Source File

SOURCE=.\Taskmgr.h
# End Source File
# Begin Source File

SOURCE=.\TipDlg.h
# End Source File
# Begin Source File

SOURCE=.\VMQuery.h
# End Source File
# Begin Source File

SOURCE=.\WindowsCore.h
# End Source File
# Begin Source File

SOURCE=.\WindowsDdk.h
# End Source File
# Begin Source File

SOURCE=.\WinObjects.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "bmp ico"
# Begin Group "Other"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\res\DefaultProcessIcon.ico
# End Source File
# Begin Source File

SOURCE=.\res\litebulb.bmp
# End Source File
# Begin Source File

SOURCE=.\module.h
# End Source File
# Begin Source File

SOURCE=.\product.h
# End Source File
# Begin Source File

SOURCE=.\res\ServiceProcessIcon.ico
# End Source File
# Begin Source File

SOURCE=.\res\SystemInfo.ico
# End Source File
# Begin Source File

SOURCE=.\res\SystemInfoToolbar.bmp
# End Source File
# Begin Source File

SOURCE=..\TaskManagerExDll\VerInfo.ver
# End Source File
# End Group
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\TaskManagerExDll.rc
# End Source File
# End Group
# Begin Group "AccessMaster"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\AccessMaster\SecurityInformation.cpp
# End Source File
# Begin Source File

SOURCE=..\AccessMaster\SecurityInformation.h
# End Source File
# Begin Source File

SOURCE=..\AccessMaster\SecurityInformationAccessRights.cpp
# End Source File
# End Group
# End Target
# End Project
# Section TaskManagerExDll : {72ADFD54-2C39-11D0-9903-00A0C91BC942}
# 	1:17:CG_IDS_DIDYOUKNOW:109
# 	1:22:CG_IDS_TIPOFTHEDAYMENU:107
# 	1:18:CG_IDS_TIPOFTHEDAY:106
# 	1:22:CG_IDS_TIPOFTHEDAYHELP:116
# 	1:19:CG_IDP_FILE_CORRUPT:115
# 	1:7:IDD_TIP:104
# 	1:13:IDB_LIGHTBULB:103
# 	1:18:CG_IDS_FILE_ABSENT:110
# 	2:17:CG_IDS_DIDYOUKNOW:CG_IDS_DIDYOUKNOW
# 	2:7:CTipDlg:CTipDlg
# 	2:22:CG_IDS_TIPOFTHEDAYMENU:CG_IDS_TIPOFTHEDAYMENU
# 	2:18:CG_IDS_TIPOFTHEDAY:CG_IDS_TIPOFTHEDAY
# 	2:12:CTIP_Written:OK
# 	2:22:CG_IDS_TIPOFTHEDAYHELP:CG_IDS_TIPOFTHEDAYHELP
# 	2:2:BH:
# 	2:19:CG_IDP_FILE_CORRUPT:CG_IDP_FILE_CORRUPT
# 	2:7:IDD_TIP:IDD_TIP
# 	2:8:TipDlg.h:TipDlg.h
# 	2:13:IDB_LIGHTBULB:IDB_LIGHTBULB
# 	2:18:CG_IDS_FILE_ABSENT:CG_IDS_FILE_ABSENT
# 	2:10:TipDlg.cpp:TipDlg.cpp
# End Section
