/*! \file hp83711bMain.cpp
 *  \author David M.S. Johnson
 *  \brief main()
 *  \section license License
 *
 *  Copyright (C) 2009 David Johnson <david.m.johnson@stanford.edu>\n
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

#include <string>
#include <iostream>

#include <ORBManager.h>
#include "hp83711bDevice.h"
#include "ConfigFile.h"


using namespace std;


ORBManager* orbManager;

int main(int argc, char **argv)
{
	orbManager = new ORBManager(argc, argv);    


	string configFilename = "hp83711b.ini"; //default

	ConfigFile configFile(configFilename);


	try{

		string deviceName = "Device Name";
		if (!(configFile.getParameter("Device Name", deviceName)))
			throw STI_Exception("Could not find \"Device Name\" in config file.");

		string ipAddress = "IP Address";
		if (!(configFile.getParameter("IP Address", ipAddress)))
			throw STI_Exception("Could not find \"IP Address\" in config file.");

		string gpibAddressString = "GPIB Address";
		if (!(configFile.getParameter("GPIB Address", gpibAddressString)))
			throw STI_Exception("Could not find \"GPIB Address\" in config file.");

		unsigned short gpibAddress;
		if(!(STI::Utils::stringToValue(gpibAddressString, gpibAddress)))
			throw STI_Exception("Could not convert GPIB address to a number");

		string logDirectory = "Log Directory";
		if (!(configFile.getParameter("Log Directory", logDirectory)))
			throw STI_Exception("Could not find \"Log Directory\" in config file.");

		string gcipAddress = "Controller IP Address";
		if (!(configFile.getParameter("Controller IP Address", gcipAddress)))
			throw STI_Exception("Could not find \"Controller IP Address\" in config file.");

		string gcModuleString = "Controller Module";
		if (!(configFile.getParameter("Controller Module", gcModuleString)))
			throw STI_Exception("Could not find \"Controller Module\" in config file.");

		unsigned short gcModule;
		if(!(STI::Utils::stringToValue(gcModuleString, gcModule)))
			throw STI_Exception("Could not convert Controller Module to a number");



		hp83711bDevice device(orbManager, deviceName, ipAddress, gpibAddress, logDirectory, gcipAddress, gcModule);
		
		device.setSaveAttributesToFile(true);

		orbManager->run();
	}
	catch (STI_Exception &e)
	{
		std::cerr << e.printMessage() << std::endl;
	}

	char a;
	std::cin >> a;
	
	return 0;
}

