cls

cd ..\..\idl\

omniidl -bcxx -C.\..\server\src\corba -Wbh=.h -Wbs=.cpp .\client.idl

pause