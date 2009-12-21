@echo off

::Library directories

set boostlibDIR=C:\Program Files\boost\boost_1_40\lib\
set omniOrblibDIR=C:\code\corba\omniORB-4.1.2\bin\x86_win32\
set pythonlibDIR=C:\WINDOWS\system32\
set xerceslibDIR=c:\code\xerces\xerces-c-3.0.1-x86-windows-vc-9.0\bin\

::EXEs to install

set stiserverEXE=server.exe
set stiRemoteFileserverEXE=RemoteFileServer.exe
set omniNamesEXE=omninames.exe

::BATs to install

set startomninamesBAT=startOmniNames.bat
set setupBAT=setup.bat
set stiserverBAT=sti_server.bat

::DLLs to install

set omniORB412DLL=omniORB412_vc9_rt.dll
set omniDynamic412DLL=omniDynamic412_vc9_rt.dll
set omnithread33DLL=omnithread33_vc9_rt.dll
set boostFilesystemDLL=boost_filesystem-vc90-mt-1_40.dll
set boostSystemDLL=boost_system-vc90-mt-1_40.dll
set pythonDLL=python25.dll
set xercesDLL=xerces-c_3_0.dll

::Shortcuts to install

set STIServerShortcut="STI Server.lnk"


:::::::::::::::::::::::::::::::::::::::::::::::::::::

::Make installation package

@echo Building the STI Server installation package

mkdir "STI Server"
cd "STI Server"

@echo Adding Setup executable and desktop shortcut...
@echo     Copying %setupBAT%...
copy ..\..\server\bin\%setupBAT%
@echo     Copying %STIServerShortcut%...
copy ..\..\server\bin\%STIServerShortcut%


@echo Adding required libraries...
mkdir lib
cd lib

:: Boost DLLS
@echo     Copying %boostFilesystemDLL%...
copy "%boostlibDIR%%boostFilesystemDLL%"

@echo     Copying %boostSystemDLL%...
copy "%boostlibDIR%%boostSystemDLL%"

:: OmniOrb DLLS
@echo     Copying %omniORB412DLL%...
copy %omniOrblibDIR%%omniORB412DLL%

@echo     Copying %omniDynamic412DLL%...
copy %omniOrblibDIR%%omniDynamic412DLL%

@echo     Copying %omnithread33DLL%...
copy %omniOrblibDIR%%omnithread33DLL%

:: Python DLL
@echo     Copying %pythonDLL%...
copy %pythonlibDIR%%pythonDLL%

:: Xerces DLL
@echo     Copying %xercesDLL%...
copy %xerceslibDIR%%xercesDLL%

cd ..


@echo Adding required executables...
mkdir bin
cd bin
@echo     Copying %stiserverEXE%...
copy ..\..\..\server\vcproject\Release\%stiserverEXE%

@echo     Copying %stiRemoteFileserverEXE%...
copy ..\..\..\server\vcproject\Release\%stiRemoteFileserverEXE%

@echo     Copying %stiserverBAT%...
copy ..\..\..\server\bin\%stiserverBAT%

cd ..


@echo Adding OmniNames
cd bin
mkdir omninames
cd omninames
mkdir log

@echo     Copying %startomninamesBAT%...
copy ..\..\..\..\server\bin\omninames\%startomninamesBAT%

@echo     Copying %omniNamesEXE%...
copy %omniOrblibDIR%%omniNamesEXE%

cd ..

cd ..
pause