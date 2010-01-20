cls

cd ..\..\idl\

omniidl -bcxx -C.\..\server\src\corba -Wbh=.h -Wbs=.cpp .\pusher.idl

cd ..\server\vcproject\

pause