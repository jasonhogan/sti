@echo off
set ipaddress=%1

IF "%ipaddress%"=="" Goto StartOmniNames

set ORBcommands=-ORBendPointPublish giop:tcp:%ipaddress%:

:StartOmniNames

del .\log\*.* /Q 
omniNames.exe -start 2809 -logdir .\log %ORBcommands%


exit