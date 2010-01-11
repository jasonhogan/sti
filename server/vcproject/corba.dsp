# Microsoft Developer Studio Project File - Name="corba" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=corba - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "corba.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "corba.mak" CFG="corba - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "corba - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "corba - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "corba - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "corba - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\..\..\device\src\corba\\" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "__WIN32__" /D "__x86__" /D _WIN32_WINNT=0x0400 /D "__NT__" /D __OSVERSION__=4 /FR /YX"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib mswsock.lib advapi32.lib omniORB411_rtd.lib omniDynamic411_rtd.lib omnithread33_rtd.lib /nologo /subsystem:console /debug /machine:I386 /out:"Debug/corbaserver.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "corba - Win32 Release"
# Name "corba - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\device\src\corba\Attribute.cpp
# End Source File
# Begin Source File

SOURCE=..\src\corba\autoserver.cpp
# End Source File
# Begin Source File

SOURCE=..\src\corba\client.cpp
# End Source File
# Begin Source File

SOURCE=..\src\corba\Control_i.cpp
# End Source File
# Begin Source File

SOURCE=..\src\corba\device.cpp
# End Source File
# Begin Source File

SOURCE=..\src\corba\DeviceConfigure_i.cpp
# End Source File
# Begin Source File

SOURCE=..\src\corba\ExpSequence_i.cpp
# End Source File
# Begin Source File

SOURCE=..\src\corba\ModeHandler_i.cpp
# End Source File
# Begin Source File

SOURCE=..\src\corba\ORBManager.cpp
# End Source File
# Begin Source File

SOURCE=..\src\corba\Parser_i.cpp
# End Source File
# Begin Source File

SOURCE=..\src\corba\RemoteDevice.cpp
# End Source File
# Begin Source File

SOURCE=..\src\corba\ServerConfigure_i.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\src\corba\STI_Server.cpp
# End Source File
# Begin Source File

SOURCE=..\src\corba\StreamingDataTransfer_i.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\device\src\corba\Attribute.h
# End Source File
# Begin Source File

SOURCE=..\src\corba\client.h
# End Source File
# Begin Source File

SOURCE=..\src\corba\Control_i.h
# End Source File
# Begin Source File

SOURCE=..\src\corba\device.h
# End Source File
# Begin Source File

SOURCE=..\src\corba\DeviceConfigure_i.h
# End Source File
# Begin Source File

SOURCE=..\src\corba\ExpSequence_i.h
# End Source File
# Begin Source File

SOURCE=..\src\corba\ModeHandler_i.h
# End Source File
# Begin Source File

SOURCE=..\src\corba\ORBManager.h
# End Source File
# Begin Source File

SOURCE=..\src\corba\Parser_i.h
# End Source File
# Begin Source File

SOURCE=..\src\corba\RemoteDevice.h
# End Source File
# Begin Source File

SOURCE=..\src\corba\ServerConfigure_i.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\src\corba\STI_Server.h
# End Source File
# Begin Source File

SOURCE=..\src\corba\StreamingDataTransfer_i.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\client.idl
# End Source File
# Begin Source File

SOURCE=..\..\device.idl
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
