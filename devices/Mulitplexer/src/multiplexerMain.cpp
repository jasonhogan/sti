
#include <ORBManager.h>
#include "multiplexerDevice.h"

ORBManager* orbManager;



int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

	string configFilename = "multiplexer.ini"; //default

	ConfigFile configFile(configFilename);

	string partnerName = "Partner Name";
	if (!(configFile.getParameter("muxedPartnerName", partnerName)))
		cout << "Could not find partner name in config file." << endl;

	string partnerChannel = "0";
	if (!(configFile.getParameter("muxedPartnerChannel", partnerChannel)))
		cout << "Could not find partner channel in config file." << endl;

	MultiplexerDevice multiplexer(orbManager, "MUX " + partnerName + " " + partnerChannel, configFilename);

	multiplexer.setSaveAttributesToFile(true);

	orbManager->run();

	return 0;
	// 350 ns, but we have 100 kHz, so wait at least 10 micro seconds, but probably 100 micro seconds
}

