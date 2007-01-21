# Microsoft Developer Studio Project File - Name="TaskManagerEx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=TaskManagerEx - Win32 Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TaskManagerEx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TaskManagerEx.mak" CFG="TaskManagerEx - Win32 Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TaskManagerEx - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "TaskManagerEx - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "TaskManagerEx - Win32 Release Unicode" (based on "Win32 (x86) Console Application")
!MESSAGE "TaskManagerEx - Win32 Debug Unicode" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/TaskManagerEx", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TaskManagerEx - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin\Release"
# PROP Intermediate_Dir "..\Intermediate\Release\TaskManagerEx"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W4 /GX /O2 /I "..\TaskManagerExDll" /D "NDEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "TASKMANEX_EXE" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 /nologo /entry:"" /subsystem:windows /machine:I386 /libpath:"..\TaskManagerExDll\Release" /OPT:NOWIN98
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ELSEIF  "$(CFG)" == "TaskManagerEx - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin\Debug"
# PROP Intermediate_Dir "..\Intermediate\Debug\TaskManagerEx"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "..\TaskManagerExDll" /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "TASKMANEX_EXE" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /entry:"" /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\TaskManagerExDll\Debug" /OPT:NOWIN98
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "TaskManagerEx - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TaskManagerEx___Win32_Release_Unicode"
# PROP BASE Intermediate_Dir "TaskManagerEx___Win32_Release_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin\Release Unicode"
# PROP Intermediate_Dir "..\Intermediate\Release Unicode\TaskManagerEx"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GX /O2 /I "..\TaskManagerExDll" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /YX"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W4 /GX /O2 /I "..\TaskManagerExDll" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "WIN32" /D "_WINDOWS" /D "TASKMANEX_EXE" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 TaskManagerEx.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"MyCRTStartup" /subsystem:windows /machine:I386 /out:"../bin/TaskManagerEx.exe" /libpath:"..\TaskManagerExDll\Release" /OPT:NOWIN98
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 /nologo /entry:"" /subsystem:windows /machine:I386 /libpath:"..\TaskManagerExDll\Release" /OPT:NOWIN98
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ELSEIF  "$(CFG)" == "TaskManagerEx - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TaskManagerEx___Win32_Debug_Unicode"
# PROP BASE Intermediate_Dir "TaskManagerEx___Win32_Debug_Unicode"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin\Debug Unicode"
# PROP Intermediate_Dir "..\Intermediate\Debug Unicode\TaskManagerEx"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "..\TaskManagerExDll" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /YX"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "..\TaskManagerExDll" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "WIN32" /D "_WINDOWS" /D "TASKMANEX_EXE" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 TaskManagerEx.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"MyCRTStartup" /subsystem:windows /debug /machine:I386 /out:"../bin/Debug/TaskManagerEx.exe" /pdbtype:sept /libpath:"..\TaskManagerExDll\Debug" /OPT:NOWIN98
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /entry:"" /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\TaskManagerExDll\Debug" /OPT:NOWIN98
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "TaskManagerEx - Win32 Release"
# Name "TaskManagerEx - Win32 Debug"
# Name "TaskManagerEx - Win32 Release Unicode"
# Name "TaskManagerEx - Win32 Debug Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\crt.cpp
# End Source File
# Begin Source File

SOURCE=.\MakeWindowTransparent.cpp
# End Source File
# Begin Source File

SOURCE=.\SplashWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TaskManagerEx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\MakeWindowTransparent.h
# End Source File
# Begin Source File

SOURCE=.\SplashWnd.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\TaskManagerExDll\TaskManagerEx.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Group "Other"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\TaskManagerExDll\res\General.ico
# End Source File
# Begin Source File

SOURCE=.\module.h
# End Source File
# Begin Source File

SOURCE=..\TaskManagerExDll\product.h
# End Source File
# Begin Source File

SOURCE=..\TaskManagerExDll\VerInfo.ver
# End Source File
# End Group
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\TaskManagerEx.rc
# End Source File
# End Group
# Begin Group "TaskManagerExDll"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\TaskManagerExDll\LoadDll.cpp
# End Source File
# Begin Source File

SOURCE=..\TaskManagerExDll\LoadDll.h
# End Source File
# Begin Source File

SOURCE=..\TaskManagerExDll\Localization.cpp
# End Source File
# Begin Source File

SOURCE=..\TaskManagerExDll\Localization.h
# End Source File
# Begin Source File

SOURCE=..\TaskManagerExDll\Options.cpp
# End Source File
# Begin Source File

SOURCE=..\TaskManagerExDll\Options.h
# End Source File
# Begin Source File

SOURCE=..\TaskManagerExDll\TaskManagerExDllExport.h
# End Source File
# Begin Source File

SOURCE=..\TaskManagerExDll\Taskmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\TaskManagerExDll\Taskmgr.h
# End Source File
# End Group
# End Target
# End Project
