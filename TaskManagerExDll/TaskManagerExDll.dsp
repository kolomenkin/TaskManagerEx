# Microsoft Developer Studio Project File - Name="TaskManagerExDll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=TaskManagerExDll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TaskManagerExDll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TaskManagerExDll.mak" CFG="TaskManagerExDll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TaskManagerExDll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TaskManagerExDll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
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
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TASKMANAGEREXDLL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TASKMANAGEREXDLL_EXPORTS" /D "WINNT" /D "_WINDLL" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 version.lib /nologo /dll /machine:I386 /out:"../bin/TaskManagerEx.dll"

!ELSEIF  "$(CFG)" == "TaskManagerExDll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TASKMANAGEREXDLL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TASKMANAGEREXDLL_EXPORTS" /D "WINNT" /D "_WINDLL" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib /nologo /dll /debug /machine:I386 /out:"../bin/Debug/TaskManagerEx.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "TaskManagerExDll - Win32 Release"
# Name "TaskManagerExDll - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutExtension.cpp
# End Source File
# Begin Source File

SOURCE=.\ApiHooks.cpp
# End Source File
# Begin Source File

SOURCE=.\DllVersion.cpp
# End Source File
# Begin Source File

SOURCE=.\FindUsedDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HookImportFunction.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadDll.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
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

SOURCE=.\TaskManagerExDll.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskManagerExDll.rc
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

SOURCE=.\DllVersion.h
# End Source File
# Begin Source File

SOURCE=.\FindUsedDlg.h
# End Source File
# Begin Source File

SOURCE=.\HookImportFunction.h
# End Source File
# Begin Source File

SOURCE=.\LoadDll.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
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

SOURCE=.\TaskManagerExDll.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "bmp ico"
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\toolbar1.bmp
# End Source File
# End Group
# End Target
# End Project
