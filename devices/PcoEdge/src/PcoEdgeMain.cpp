/*! \file PcoEdgeMain.cpp
 *  \author Brannon Klopfer
 *  \brief Source-file for the class GenericDevice
 *  \section license License
 *
 *  Copyright (C) 2012 Brannon Klopfer <bbklopfer@stanford.edu>
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

#include <iostream>
#include <sstream>
#include <ORBManager.h>
#include <boost/program_options.hpp>
#include <boost/range/algorithm/remove_if.hpp>

#include "PcoEdgeDevice.hpp"
#include "PcoEdgeCamera.hpp"

// TODO: Use standard error codes.
#define ERROR_IN_COMMAND_LINE 1
#define ERROR_STI_DEVICE_NOT_INITIALIZED 2

using namespace std;

int main(int argc, char* argv[])
{
	string name = "PcoEdge";
	string address = "unknown";
	string logdir = "";

	// Let ORBManager eat the arguments we don't want.
	ORBManager* orbManager;
	orbManager = new ORBManager(argc, argv);
	argc = orbManager->getArgc();
	argv = orbManager->getArgv();

	// Parse command line arguments
	namespace po = boost::program_options;
	po::variables_map vm;
	po::options_description desc("Options");
	desc.add_options()
		("help", "FIXME DO THIS")
		("name,n", po::value<string>(), "name of device")
		("address,a", po::value<string>(), "device address (e.g., hostname)")
		("logdir,l", po::value<string>(), "log directory");
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		if (vm.count("help")) {
			cout << "Look at the code..." << endl
				 << desc << endl;
		}
		po::notify(vm); // Throw error if there was a problem.
	} catch (po::error& e) {
		cerr << "ERROR: " << e.what() << endl << endl << desc << endl;

		return ERROR_IN_COMMAND_LINE;
	}

	if (!vm["name"].empty() && !vm["name"].as<string>().empty())
		name = vm["name"].as<string>();
	if (!vm["address"].empty() && !vm["address"].as<string>().empty())
		address = vm["address"].as<string>();
	if (!vm["logdir"].empty() && !vm["logdir"].as<string>().empty())
		logdir = vm["logdir"].as<string>();

	PcoEdgeDevice cam(orbManager, name, address, 0, logdir);

	if (cam.initialized) {
		orbManager->run();
	} else {
		cerr << "Failed to initialize " << name << "." << endl;

		return ERROR_STI_DEVICE_NOT_INITIALIZED;
	}

	return 0;
}





