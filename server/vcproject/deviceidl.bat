cls

omniidl -bcxx -C.\..\src\corba -Wbh=.h -Wbs=.cpp .\..\..\idl\device.idl

copy .\..\src\corba\device.h .\..\..\device\include
copy .\..\src\corba\device.cpp .\..\..\device\src

pause