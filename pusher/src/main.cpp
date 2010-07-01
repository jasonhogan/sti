

#include <iostream>
#include <ORBManager.h>

using namespace std;

ORBManager* orbManager;

int main(int argc, char **argv)
{
	orbManager = new ORBManager(argc, argv);

	orbManager->run();
	
	return 0;
}