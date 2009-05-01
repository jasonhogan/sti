@echo off

::Library directories

set boostlibDIR=C:\Program Files\boost\boost_1_36_0\lib\
set omniOrblibDIR=C:\code\corba\omniORB-4.1.2\bin\x86_win32\

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
set boostFilesystemDLL=boost_filesystem-vc90-mt-1_36.dll
set boostSystemDLL=boost_system-vc90-mt-1_36.dll

::Shortcuts to install

set STIServerShortcut="STI Server.lnk"


:::::::::::::::::::::::::::::::::::::::::::::::::::::

::Make installation package

@echo Building the STI Server installation package

mkdir "STI Server"
cd "STI Server"

@echo Adding Setup executable and desktop shortcut...
copy ..\..\server\bin\%setupBAT%
copy ..\..\server\bin\%STIServerShortcut%


@echo Adding required libraries...
mkdir lib
cd lib
copy "%boostlibDIR%%boostFilesystemDLL%"
copy "%boostlibDIR%%boostSystemDLL%"

copy %omniOrblibDIR%%omniORB412DLL%
copy %omniOrblibDIR%%omniDynamic412DLL%
copy %omniOrblibDIR%%omnithread33DLL%
cd ..


@echo Adding required executables...
mkdir bin
cd bin
copy ..\..\..\server\vcproject\Release\%stiserverEXE%
copy ..\..\..\server\vcproject\Release\%stiRemoteFileserverEXE%
copy ..\..\..\server\bin\%stiserverBAT%
cd ..


@echo Adding OmniNames
cd bin
mkdir omninames
cd omninames
mkdir log
copy ..\..\..\..\server\bin\omninames\%startomninamesBAT%
copy %omniOrblibDIR%%omniNamesEXE%
cd ..

cd ..
pause