
#include <ORBManager.h>
#include "HPSidebandLockDevice.h"
#include "CalibrationResults.h"

ORBManager* orbManager;

int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

	string configFilename1 = "hpSidebandLockDevice1.ini"; //default
	string configFilename2 = "hpSidebandLockDevice2.ini"; //default

	//Shared calibration results
	CalibrationResults_ptr calibrationResults = CalibrationResults_ptr(new CalibrationResults());

	HPSidebandLockDevice sidebandLockDevice1(orbManager, "High Power Sideband Lock 1", configFilename1, calibrationResults);
	HPSidebandLockDevice sidebandLockDevice2(orbManager, "High Power Sideband Lock 2", configFilename2, calibrationResults);

	sidebandLockDevice1.setSaveAttributesToFile(true);
	sidebandLockDevice2.setSaveAttributesToFile(true);

	//if (sidebandLockDevice1.isInitialized())	
	if (sidebandLockDevice1.isInitialized() && sidebandLockDevice2.isInitialized())
		orbManager->run();
	else
		cout << "Error initializing high power sideband lock" << endl;

	return 0;
}

