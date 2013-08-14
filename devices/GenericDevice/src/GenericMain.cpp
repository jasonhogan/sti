/*! \file GenericMain.cpp
 *  \author Brannon Klopfer
 *  \author David M.S. Johnson
 *  \author Susannah Dickerson
 *  \brief Source-file for the class GenericDevice
 *  \section license License
 *
 *  Copyright (C) 2012 Susannah Dickerson <sdickers@stanford.edu>,
 *  Brannon Klopfer <bbklopfer@stanford.edu>
 *
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  The STI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The STI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the STI.  If not, see <http://www.gnu.org/licenses/>.
 */

// Invocation (on Windows) should be something like:
// .\genericDevice.exe --xml DeviceFile.xml --ini ConfigFile.ini -- -ORBendPointPublish giop:tcp:%ipaddress%: -ORBInitRef NameService=corbaname::%ipaddress%:2809 %GLOBAL_OPTS%

#include <string>
#include <iostream>

#include <ORBManager.h>
#include "GenericDevice.hpp"

// For command line parsing, etc.
#include <boost/program_options.hpp>
#include <boost/range/algorithm/remove_if.hpp>

#define ERROR_IN_COMMAND_LINE 1

using namespace std;

ORBManager* orbManager;

int main(int argc, char **argv)
{
	string configFileName;
	string myXMLDuderFileName;

	// Parse command line arguments
	namespace po = boost::program_options;
	po::variables_map vm;
	po::options_description desc("Options");
	desc.add_options()
		("help", "FIXME DO THIS")
		("xml,x", po::value<string>()->required(), "XML file to use.")
		("ini,i", po::value<string>()->required(), "INI file to use.")
		// TODO: This appears to magically work. I don't exactly know why.
		("", po::value<string>(), "Pass arguments to O M N I O R B.");
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		if (vm.count("help")) {
			cout << "Look at the code..." << endl
				 << desc << endl;
		}

		po::notify(vm); // Throw error if there was a problem.
	}
	catch (po::error& e)
	{
		cerr << "ERROR: " << e.what() << endl << endl << desc << endl;

		return ERROR_IN_COMMAND_LINE;
	}

	// Grab command line arguments
	myXMLDuderFileName = vm["xml"].as<string>(); 
	configFileName = vm["ini"].as<string>();

	orbManager = new ORBManager(argc, argv);

	// TODO: Optionally overwrite from command line (and make .ini optional)
	string devAddr_str;
	unsigned short devAddr;
	string ipaddr;
	string initScript;

	ConfigFile configFile(configFileName);
	configFile.getParameter("Addr", devAddr_str);
	configFile.getParameter("IP Address", ipaddr);
	configFile.getParameter("InitScript", initScript);

	// NB: There's some funky business going on. The devAddr passed to
	// the GenericDevice is just used for UI purposes. Clean this up.
	devAddr_str.erase(boost::remove_if(devAddr_str, ::isalpha), devAddr_str.end());

	// string to int:
	istringstream(devAddr_str) >> devAddr;

	GenericDeviceConfig appConfig;
	appConfig.readConfigFile(myXMLDuderFileName);
	appConfig.parseDeviceSpecificConfig(configFile);

	// I think we should have two or three names: a device name (hp34401a), of which there can be an arbitrary number connected to the system.
	// We should also have a human-readable name (e.g., "vtip dmm") and a computer-friendly name for use in MATLAB scripts, so we can do stuff
	// like "sti.vtip_dmm.read()" or similar. These names would be unique.
	GenericDevice duder(orbManager, appConfig.name, ipaddr, devAddr, initScript, &appConfig, "", ipaddr);

	if (duder.initialized)
		orbManager->run();
	else
		std::cerr << "Failed to initialize device." << std::endl;

	return 0;
}

