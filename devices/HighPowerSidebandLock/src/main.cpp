
#include <ORBManager.h>
#include "HPSidebandLockDevice.h"

ORBManager* orbManager;

int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    


	string configFilename = "hpSidebandLockDevice.ini"; //default

	HPSidebandLockDevice sidebandLockDevice(orbManager, "High Power Sideband Lock", configFilename);

	sidebandLockDevice.setSaveAttributesToFile(true);

	if (sidebandLockDevice.initialized)
		orbManager->run();
	else
		cout << "Error initializing high power sideband lock" << endl;

	return 0;
}

