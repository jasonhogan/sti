
#include <ORBManager.h>
#include "HighPowerIntensityLockDevice.h"

ORBManager* orbManager;



int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

	HighPowerIntensityLockDevice highPowerIntensityLock1(orbManager, "High Power Intensity Lock 1", "EPMezzanine1.stanford.edu", 0);
//	HighPowerIntensityLockDevice highPowerIntensityLock2(orbManager, "High Power Intensity Lock 2", "EPMezzanine1.stanford.edu", 1);

	highPowerIntensityLock1.setSaveAttributesToFile(true);
//	highPowerIntensityLock2.setSaveAttributesToFile(true);

	orbManager->run();

	return 0;
}

