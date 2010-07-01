cls

cd ..\..\idl\

omniidl -bcxx -C.\..\fileserver\src -Wbh=.h -Wbs=.cpp .\FileServer.idl

pause