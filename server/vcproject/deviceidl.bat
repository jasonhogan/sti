cls

omniidl -bcxx -C.\..\src\corba -Wbh=.h -Wbs=.cpp ..\..\idl\device.idl

copy .\..\src\corba\device.h .\..\..\device\include

pause