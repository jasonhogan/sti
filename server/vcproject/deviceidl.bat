cls

omniidl -bcxx -C.\..\src\corba -Wbh=.h -Wbs=.cpp ..\..\device.idl

copy .\..\src\corba\device.* .\..\..\device\src\corba

pause