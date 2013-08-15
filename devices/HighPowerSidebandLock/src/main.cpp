
#include <ORBManager.h>
#include "HPSidebandLockDevice.h"
#include "CalibrationResults.h"

ORBManager* orbManager;

int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

	string configFilename = "hpSidebandLockDevice.ini"; //default

	//Shared calibration results
	CalibrationResults_ptr calibrationResults = CalibrationResults_ptr(new CalibrationResults());

	HPSidebandLockDevice sidebandLockDevice(orbManager, "High Power Sideband Lock", configFilename, calibrationResults);

	sidebandLockDevice.setSaveAttributesToFile(true);

	if (sidebandLockDevice.isInitialized())
		orbManager->run();
	else
		cout << "Error initializing high power sideband lock" << endl;

	return 0;
}

