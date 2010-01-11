@echo off

@echo This will install STI Server on this computer.
@echo --Required DLLs will be copied to the Windows system folder.
@echo --A shortcut will be placed on the Desktop.
set /P install=Proceed with installation? (Y/n) 
IF %install%==n Exit

@echo Installing...

@echo on
copy .\lib\*.* c:\windows\system32\
copy "STI Server.lnk" "C:\Documents and Settings\Owner\Desktop"
@echo off

pause
exit