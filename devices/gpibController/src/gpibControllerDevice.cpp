/*! \file gpib_hub_device.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class gpib_hub
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



#include "gpibControllerDevice.h"
#include "ENET_GPIB_device.h"



gpibControllerDevice::gpibControllerDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{
	gpibController = new ENET_GPIB_device(); 
}

gpibControllerDevice::~gpibControllerDevice()
{
}


void gpibControllerDevice::defineAttributes() 
{

}

void gpibControllerDevice::refreshAttributes() 
{

}

bool gpibControllerDevice::updateAttribute(string key, string value)
{

	//string result = partnerDevice("gpibController").execute("1 0 MEAS:blahblahblah 1"); //usage: partnerDevice("lock").execute("--e1");
	return false;
}

void gpibControllerDevice::defineChannels()
{
}

bool gpibControllerDevice::writeChannel(const RawEvent& Event)
{
	return false;
}

bool gpibControllerDevice::readChannel(ParsedMeasurement& Measurement)
{
	return false;
}

void gpibControllerDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	
}
void gpibControllerDevice::definePartnerDevices()
{
	//addPartnerDevice("gpibController", "li-gpib.stanford.edu", 12, "gpib"); //local name (shorthand), IP address, module #, device name as defined in main function
}

void gpibControllerDevice::stopEventPlayback()
{
}

std::string gpibControllerDevice::execute(int argc, char **argv)
{
	int primaryAddress;
	int secondaryAddress;
	string command;
	int query = 0; //true (1) or false (0) if the command is expecting a response
	bool querySuccess;

	//command comes as "primary_address secondary_address command query(t/f)?"
	if(argc == 5)
	{
		command = string(argv[3]);
		querySuccess = stringToValue(argv[4], query);
		std::cerr << "command is: " << command << std::endl;
	}
	else if(argc == 6)
	{
		command = string(argv[3]) + string(argv[4]);
		querySuccess = stringToValue(string(argv[5]), query);
		std::cerr << "command is: " << command << std::endl;
	}
	else
		return ""; //command needs to contain 3 pieces of information

	bool primaryAddressSuccess = stringToValue(argv[1], primaryAddress);
	bool secondaryAddressSuccess = stringToValue(argv[2], secondaryAddress);
	

	if(query == 1 && querySuccess)
	{
		//query device
		gpibController->Query_Device(primaryAddress, secondaryAddress, const_cast<char*>(command.c_str()), result, 100);
		std::cerr << "result is: " << result << std::endl;
		return result;
	}
	else if(!query && querySuccess)
	{
		//command device
		gpibController->Command_Device(primaryAddress, secondaryAddress, const_cast<char*>(command.c_str()), result, 100);
		std::cerr << "result is: " << "worked" << std::endl;
		return "1";
	}
	else
		return "";
	
}
bool gpibControllerDevice::deviceMain(int argc, char **argv)
{
	return false;
}