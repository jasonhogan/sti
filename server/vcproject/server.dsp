# Microsoft Developer Studio Project File - Name="server" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=server - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "server.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "server.mak" CFG="server - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "server - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "server - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "server - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "c:\python25" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "__WIN32__" /D "__x86__" /D _WIN32_WINNT=0x0400 /D "__NT__" /D __OSVERSION__=4 /D "HAVE_PYTHON2_5_PYTHON_H" /D "HAVE_PYTHON2_5_STRUCTMEMBER_H" /D "HAVE_LIBPYTHON2_5" /YX"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib mswsock.lib advapi32.lib omniORB411_rt.lib omniDynamic411_rt.lib omnithread33_rt.lib python25.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "server - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "c:\python25" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "__WIN32__" /D "__x86__" /D _WIN32_WINNT=0x0400 /D "__NT__" /D __OSVERSION__=4 /D "HAVE_PYTHON2_5_PYTHON_H" /D "HAVE_PYTHON2_5_STRUCTMEMBER_H" /D "HAVE_LIBPYTHON2_5" /FR /YX"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib mswsock.lib advapi32.lib omniORB411_rtd.lib omniDynamic411_rtd.lib omnithread33_rtd.lib python25.lib /nologo /subsystem:console /debug /machine:I386 /out:"Debug/sti_server.exe" /pdbtype:sept /libpath:"c:\python25\libs"

!ENDIF 

# Begin Target

# Name "server - Win32 Release"
# Name "server - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "python source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\python\antikbdint.cpp
# End Source File
# Begin Source File

SOURCE=..\src\python\brdobject.cpp
# End Source File
# Begin Source File

SOURCE=..\src\python\chobject.cpp
# End Source File
# Begin Source File

SOURCE=..\src\python\listenerobject.cpp
# End Source File
# Begin Source File

SOURCE=..\src\python\parsedevent.cpp
# End Source File
# Begin Source File

SOURCE=..\src\python\parsedpos.cpp
# End Source File
# Begin Source File

SOURCE=..\src\python\parsedvar.cpp
# End Source File
# Begin Source File

SOURCE=..\src\python\parser.cpp
# End Source File
# Begin Source File

SOURCE=..\src\python\timing.cpp
# End Source File
# End Group
# Begin Group "corba source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\corba\client.cpp
# End Source File
# Begin Source File

SOURCE=..\src\corba\Control_i.cpp
# End Source File
# Begin Source File

SOURCE=..\src\corba\ExpSequence_i.cpp
# End Source File
# Begin Source File

SOURCE=..\src\corba\ModeHandler_i.cpp
# End Source File
# Begin Source File

SOURCE=..\src\corba\Parser_i.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\server.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "python headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\python\antikbdint.h
# End Source File
# Begin Source File

SOURCE=..\src\python\brdobject.h
# End Source File
# Begin Source File

SOURCE=..\src\python\chobject.h
# End Source File
# Begin Source File

SOURCE=..\src\python\listenerobject.h
# End Source File
# Begin Source File

SOURCE=..\src\python\parsedevent.h
# End Source File
# Begin Source File

SOURCE=..\src\python\parsedpos.h
# End Source File
# Begin Source File

SOURCE=..\src\python\parsedvar.h
# End Source File
# Begin Source File

SOURCE=..\src\python\parser.h
# End Source File
# Begin Source File

SOURCE=..\src\python\timing.h
# End Source File
# End Group
# Begin Group "corba headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\corba\client.h
# End Source File
# Begin Source File

SOURCE=..\src\corba\Control_i.h
# End Source File
# Begin Source File

SOURCE=..\src\corba\ExpSequence_i.h
# End Source File
# Begin Source File

SOURCE=..\src\corba\ModeHandler_i.h
# End Source File
# Begin Source File

SOURCE=..\src\corba\Parser_i.h
# End Source File
# Begin Source File

SOURCE=..\src\corba\STI_Servant.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\client.idl
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
