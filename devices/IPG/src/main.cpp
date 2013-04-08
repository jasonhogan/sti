
#include <ORBManager.h>
#include "IPG_FiberAmpDevice.h"

ORBManager* orbManager;



int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

	int comPortNumber1 = 5;
	int comPortNumber2 = 6;

	IPG_FiberAmpDevice fiberAmp1(orbManager, "IPG Fiber Amp 1", "EPMezzanine1.stanford.edu", 0, comPortNumber1);
	IPG_FiberAmpDevice fiberAmp2(orbManager, "IPG Fiber Amp 2", "EPMezzanine1.stanford.edu", 1, comPortNumber2);

	fiberAmp1.setSaveAttributesToFile(true);
	fiberAmp2.setSaveAttributesToFile(true);

	orbManager->run();

	return 0;
}

