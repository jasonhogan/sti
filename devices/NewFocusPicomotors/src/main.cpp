
#include <ORBManager.h>
#include "PicomotorsDevice.h"

ORBManager* orbManager;



int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

	string configFilename = "picomotors.ini"; //default
	ConfigFile configFile(configFilename);

//	unsigned COMport = 12;
//	PicomotorsDevice picomotors(orbManager, "Picomotors", "171.64.56.96", 0, COMport);
	
	PicomotorsDevice picomotors(orbManager, configFile);

	orbManager->run();

	return 0;
}

