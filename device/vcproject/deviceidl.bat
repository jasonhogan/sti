cls

omniidl -bcxx -C.\..\src\corba -Wbh=.h -Wbs=.cpp ..\..\device.idl

copy .\..\src\corba\device.* .\..\..\server\src\corba

pause