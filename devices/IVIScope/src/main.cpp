
#include <ORBManager.h>
#include "IVIScopeDevice.h"

ORBManager* orbManager;



int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

	string configFilename = "iviScope.ini"; //default

	ConfigFile configFile(configFilename);

	string logicalName = "IVI Scope";
	if (!(configFile.getParameter("logicalName", logicalName)))
		cout << "Could not find logical name in config file." << endl;

	IVIScopeDevice scope1(orbManager, logicalName, configFilename);

	orbManager->run();

	return 0;
}

