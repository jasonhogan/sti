cls

cd ..\..\idl\

omniidl -bcxx -C.\..\device\src -Wbh=.h -Wbs=.cpp .\device.idl

cd ..\device\vcproject\

move .\..\src\device.h .\..\include

pause