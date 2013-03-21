
#include <ORBManager.h>
#include "PhaseMatrixDevice.h"

ORBManager* orbManager;



int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

	int comPortNumber = 3;

	PhaseMatrixDevice phaseMatrix(orbManager, "PhaseMatrix", "EPMezzanine1.stanford.edu", 0, comPortNumber);

	orbManager->run();

	return 0;
}

