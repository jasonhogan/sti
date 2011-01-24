/*! \file autoserver.cpp
 *  \author Jason Hogan
 *  \brief A test server which automatically mounts new STI_Devices.
 */





#include <omniORB4/omniURI.h>
#include <string>
#include <iostream>

#include <ORBManager.h>
#include "corba/STI_Server.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

using namespace std;

ORBManager* orbManager;
STI_Server* server;

int main(int argc, char **argv)
{
	_CrtDumpMemoryLeaks();

	orbManager = new ORBManager(argc, argv);
	server = new STI_Server("STI Server", orbManager);



//	cerr << orbManager->errMsg() << endl;

	orbManager->run();
	
	return 0;
}
