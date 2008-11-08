cls

cd ..\..\idl\

omniidl -bcxx -C.\..\server\src\corba -Wbh=.h -Wbs=.cpp .\client.idl

cd ..\server\vcproject\

pause