cls

::set pythonpath=C:\Users\Jason\Code\python\Python-2.5.5\Lib

cd ..\..\idl\

omniidl -bcxx -C.\..\device\src -Wbh=.h -Wbs=.cpp .\device.idl

cd ..\device\vcproject\

move .\..\src\device.h .\..\include

pause