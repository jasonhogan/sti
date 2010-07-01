cls

cd ..\..\idl\

::omniidl -bcxx -C.\..\server\src\corba -Wbh=.h -Wbs=.cpp .\pusher.idl
omniidl -bcxx -C.\..\pusher\src -Wbh=.h -Wbs=.cpp .\pusher.idl

::cd ..\server\vcproject\


::copy .\..\src\corba\pusher.h .\..\..\pusher\src
::copy .\..\src\corba\pusher.cpp .\..\..\pusher\src

pause