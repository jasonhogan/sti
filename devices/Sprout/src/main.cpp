
#include <ORBManager.h>
#include "SproutDevice.h"

ORBManager* orbManager;



int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

	SproutDevice sprout(orbManager, "Sprout", "epdesktop1.stanford.edu", 0, 6);

	orbManager->run();

	return 0;
}

