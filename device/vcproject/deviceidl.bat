cls

omniidl -bcxx -C.\..\src -Wbh=.h -Wbs=.cpp ..\..\idl\device.idl

move .\..\src\device.h .\..\include

pause