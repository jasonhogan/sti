cls

::set pythonpath=C:\Users\Jason\Code\python\Python-2.5.5\Lib


cd ..\..\idl\


omniidl -bcxx -C.\..\device\src -Wbh=.h -Wbs=.cpp .\client.idl

cd ..\device\vcproject\

move .\..\src\client.h .\..\include

pause