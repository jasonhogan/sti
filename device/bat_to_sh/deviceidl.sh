#!/bin/sh
cd ../../idl/


omniidl -bcxx -C./../device/src -Wbh=.h -Wbs=.cpp ./device.idl

cd ../device/vcproject/

mv ./../src/device.h ./../include
