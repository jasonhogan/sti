cls

cd ..\..\idl\

omniidl -bcxx -C.\..\pusher\src -Wbh=.h -Wbs=.cpp .\pusher.idl

copy .\..\pusher\src\pusher.h .\..\device\include
copy .\..\pusher\src\pusher.cpp .\..\device\src

pause