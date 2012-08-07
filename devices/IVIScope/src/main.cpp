
#include <ORBManager.h>
#include "IVIScopeDevice.h"

ORBManager* orbManager;



int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

	IVIScopeDevice scope1(orbManager, "Pit Scope", "Trident.stanford.edu", 0);

	orbManager->run();

	return 0;
}

