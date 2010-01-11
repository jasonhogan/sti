cls

omniidl -bcxx -C.\..\src\corba -Wbh=.h -Wbs=.cpp .\..\..\idl\orbTypes.idl

copy .\..\src\corba\orbTypes.h .\..\..\device\include
copy .\..\src\corba\orbTypes.cpp .\..\..\device\src

pause