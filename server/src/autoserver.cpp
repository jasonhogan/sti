/*! \file autoserver.cpp
 *  \author Jason Hogan
 *  \brief main() for the STI Server
 */

#include <ORBManager.h>
#include "corba/STI_Server.h"

int main(int argc, char **argv)
{
	ORBManager* orbManager = new ORBManager(argc, argv);
	STI_Server* server = new STI_Server("STI Server", orbManager);

	orbManager->run();
	
	return 0;
}
