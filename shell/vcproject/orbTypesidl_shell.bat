cls

cd ..\..\idl\

omniidl -bcxx -C.\..\shell\src\corba -Wbh=.h -Wbs=.cpp .\orbTypes.idl

cd ..\shell\vcproject\

pause