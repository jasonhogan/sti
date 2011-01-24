

#include <string>
#include <ORBManager.h>
#include "testApp.h"

ORBManager* orbManager;

int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);


	std::string configFilename = "testApp.ini"; //default

	if(argc > 1)
	{
		configFilename = string( argv[1] );
	}

	TestApp testApp(orbManager, configFilename);

	orbManager->run();

	return 0;
}


