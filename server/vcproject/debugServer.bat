@echo off

::The STI Server's IP Address:
set ipaddress=192.168.1.102

::Start omniNames, the remote file server, and the STI server
cd .\..\..\bin\STI Server\bin\omninames
start /min .\startOmniNames.bat %ipaddress%
cd .\..
start /min .\RemoteFileServer.exe -ORBendPointPublish giop:tcp:%ipaddress%: -ORBInitRef NameService=corbaname::%ipaddress%:2809


.\..\..\..\server\vcproject\Debug\sti_server.exe -ORBendPointPublish giop:tcp:%ipaddress%: -ORBInitRef NameService=corbaname::%ipaddress%:2809