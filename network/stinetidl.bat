cls

::set pythonpath=C:\Users\Jason\Code\python\Python-2.5.5\Lib

::cd ..\..\idl\

omniidl -bcxx -C.\src -Wbh=.h -Wbs=.cpp .\stinet.idl

move stinet.h .\include

pause