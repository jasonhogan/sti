cls

cd ..\..\idl\

omniidl -bcxx -C.\..\server\src\corba -Wbh=.h -Wbs=.cpp .\device.idl

cd ..\server\vcproject\

copy .\..\src\corba\device.h .\..\..\device\include
copy .\..\src\corba\device.cpp .\..\..\device\src

pause