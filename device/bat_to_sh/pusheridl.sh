#/bin/sh
cd ../../idl

omniidl -bcxx -C./../pusher/src -Wbh=.h -Wbs=.cpp ./pusher.idl

cp ./../pusher/src/pusher.h ./../device/include
cp ./../pusher/src/pusher.cpp ./../device/src
