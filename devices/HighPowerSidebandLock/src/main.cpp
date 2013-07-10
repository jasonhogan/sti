
#include <ORBManager.h>
#include "HPSidebandLockDevice.h"

ORBManager* orbManager;

int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

	HPSidebandLockDevice sidebandLockDevice(orbManager, "High Power Sideband Lock", "EPMezzanine1.stanford.edu", 0);

	sidebandLockDevice.setSaveAttributesToFile(true);

	orbManager->run();

	return 0;
}

