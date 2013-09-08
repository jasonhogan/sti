
#include "StartDevice.h"
#include "CleverScopeDevice.h"
#include <ORBManager.h>


void makeAndRunDeviceWithHandle(UnmanagedCallback callback, 
								UnmanagedCallback callbackA, UnmanagedCallback callbackB,
								STIDeviceCLRBridgeWrapper* deviceWrapper)
{
	//Fake args for ORBManager
	int argc = 1;
	char* argv[] = {""};

	ORBManager* orbManager = new ORBManager(argc, argv);
	
	CleverScopeDevice device(orbManager, "Clever Scope", "localhost", 0, callback);

	device.addHandleChannelA(callbackA);
	device.addHandleChannelB(callbackB);

	//Connect the supplied wrapper with the actual device bridge pointer.
	deviceWrapper->setDeviceBridge(&device);

	orbManager->run();
	
	//If device exits for some reason, clear the bridge pointer.
	deviceWrapper->setDeviceBridge(NULL);
}
