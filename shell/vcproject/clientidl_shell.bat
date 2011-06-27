cls

cd ..\..\idl\

omniidl -bcxx -C.\..\shell\src\corba -Wbh=.h -Wbs=.cpp .\client.idl

cd ..\shell\vcproject\

pause