
#include <ORBManager.h>
#include "HighFreqSidebandLockDevice.h"
#include "CalibrationResults.h"

ORBManager* orbManager;

int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

	string configFilename1 = "HighFreqSidebandLockDevice1.ini"; //default
	string configFilename2 = "HighFreqSidebandLockDevice2.ini"; //default

	//Shared calibration results
	CalibrationResults_ptr calibrationResults = CalibrationResults_ptr(new CalibrationResults());

	HighFreqSidebandLockDevice sidebandLockDevice1(orbManager, "High Freq Sideband Lock 1", configFilename1, calibrationResults);
	HighFreqSidebandLockDevice sidebandLockDevice2(orbManager, "High Freq Sideband Lock 2", configFilename2, calibrationResults);

	sidebandLockDevice1.setSaveAttributesToFile(true);
	sidebandLockDevice2.setSaveAttributesToFile(true);

	//if (sidebandLockDevice1.isInitialized())	
	if (sidebandLockDevice1.isInitialized() && sidebandLockDevice2.isInitialized())
		orbManager->run();
	else
		cout << "Error initializing HighFreqSidebandLock" << endl;

	return 0;
}

