
#include <ORBManager.h>
#include "PicomotorsDevice.h"

ORBManager* orbManager;



int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

	unsigned COMport = 12;
	PicomotorsDevice picomotors(orbManager, "Picomotors", "171.64.56.96", 0, COMport);

	orbManager->run();

	return 0;
}

