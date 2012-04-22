
#include <ORBManager.h>
#include "PicomotorsDevice.h"

ORBManager* orbManager;



int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

	unsigned COMport = 3;
	PicomotorsDevice picomotors(orbManager, "Picomotors", "epdesktop1.stanford.edu", 0, COMport);

	orbManager->run();

	return 0;
}

