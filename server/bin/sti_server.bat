@echo off

::The STI Server's IP Address:
set ipaddress=171.64.57.183


::Start omniNamed, the remote file server, and the STI server
cd .\omninames
start /min .\startOmniNames.bat %ipaddress%
cd .\..
start /min .\RemoteFileServer.exe -ORBendPointPublish giop:tcp:%ipaddress%: -ORBInitRef NameService=corbaname::%ipaddress%:2809
.\server.exe -ORBendPointPublish giop:tcp:%ipaddress%: -ORBInitRef NameService=corbaname::%ipaddress%:2809