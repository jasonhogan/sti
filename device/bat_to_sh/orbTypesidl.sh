#/bin/sh
omniidl -bcxx -C./../src -Wbh=.h -Wbs=.cpp ../../idl/orbTypes.idl

mv ./../src/orbTypes.h ./../include
