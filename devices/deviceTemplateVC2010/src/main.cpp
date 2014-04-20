
#include <ORBManager.h>
#include "ExampleDevice.h"

int main(int argc, char* argv[])
{
	//The ORBManager manages the connection of your device to the STI Server over the network.
	ORBManager* orbManager = new ORBManager(argc, argv);

	//Create your device instance, using an initialization file for the device identification information.
	//The "myDevice.ini" file must contain the device's name, address and module number.
	//Note: There are alternative device constructors available, but initialization files are preferred.
	ExampleDevice exampleDevice(orbManager, "myDevice.ini");

	//Start the ORBManager. This makes the device attempt to register with the server.
	//The run() function does not return until the device is killed.
	orbManager->run();

	return 0;
}

