cls

omniidl -bcxx -C.\..\src -Wbh=.h -Wbs=.cpp ..\..\idl\orbTypes.idl

move .\..\src\orbTypes.h .\..\include

pause