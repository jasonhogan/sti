/*! \file littleTableControllerDevice.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class littleTableControllerDevice
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



#include "littleTableControllerDevice.h"

littleTableControllerDevice::littleTableControllerDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{ 
	//initialize values
	gpibID = "Have Not Queried";
}

littleTableControllerDevice::~littleTableControllerDevice()
{

}


void littleTableControllerDevice::defineAttributes() 
{
	addAttribute("GPIB ID", gpibID); //response to the IDN? query
}

void littleTableControllerDevice::refreshAttributes() 
{
	setAttribute("GPIB ID", gpibID); //response to the IDN? query
}

bool littleTableControllerDevice::updateAttribute(string key, string value)
{
	//converts desired command into GPIB command string and executes via gpib controller partner device
	double tempDouble;
	std::string tempString;
	double tempValue = 0;
	bool successDouble = stringToValue(value, tempDouble);
	bool commandSuccess;
	bool success = false;
	std::string result;
	std::string commandString;



	return success;
}
void littleTableControllerDevice::definePartnerDevices()
{
	addPartnerDevice("mux", "li-gpib.stanford.edu", 5, "Agilent34970a"); //local name (shorthand), IP address, module #, device name as defined in main function
	addPartnerDevice("vortex", "li-gpib.stanford.edu", 1, "Vortex6000");
	addPartnerDevice("lockProgram", "171.64.56.254", 13, "lock"); 
	//hp func x2
	//agilent func

}

bool littleTableControllerDevice::deviceMain(int argc, char **argv)
{
	bool success = true;
	std::string measurement;
	std::string commandString;

	while(success)
	{
		if(externalVortexLoopEnabled) //if external vortex loop is enabled...
		{
			//determine avg. value of vortex control signal
			result = partnerDevice("mux").execute(commandString); //usage: partnerDevice("lock").execute("--e1");
		}

		if(continuouslyMonitorMux) //loops through channels in mux which have been selected to be monitored
		{
			//
		}
	}

	

	
	return false;
}
