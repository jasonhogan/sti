/*! \file STI_Device_Template.cpp
 *  \author Susannah Dickerson
 *  \brief Template for STI_Devices
 *  \section license License
 *
 *  Copyright (C) 2009 Susannah Dickerson <sdickers@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *	This structure shamlessly derived from source code originally by Jason
 *	Hogan <hogan@stanford.edu> and David M.S. Johnson <david.m.johnson@stanford.edu>
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



#include "STI_Device_Template.h"


MyDevice::MyDevice(ORBManager*    orb_manager, 
					   std::string    DeviceName, 
					   std::string    Address, 
					   unsigned short ModuleNumber) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{
	//Initialization of device
}

MyDevice::~lockDevice()
{
}


bool MyDevice::deviceMain(int argc, char **argv)
{

	refreshAttributes();
	
	return false;
}

void MyDevice::defineAttributes() 
{
	addAttribute("Attribute Name v1", "Initial Option", "Option 1, Option 2, ...");
	addAttribute("Attribute Name v2", some_initial_number);
}

void MyDevice::refreshAttributes() 
{

	setAttribute("Attribute Name v1", "Option to set");   //figuring out which option to set 
														//often requires a switch on some parameter
	setAttribute("Attribute Name v2", some_other_number); 

}

bool MyDevice::updateAttribute(string key, string value)
{
	double tempDouble;  //the value entered, converted to a number
	int tempInt;

	bool successDouble = stringToValue(value, tempDouble);
	bool successInt = stringToValue(value, tempInt);

	bool success = successDouble || successInt;

	if(key.compare("Attribute Name v1") == 0)
	{
		success = true;
		
		if(value.compare("Option 1") == 0)
		{
			set appropriate variable;
			std::cerr << "Attribute Name v1 = Option 1" << std::endl;
		}
		else if(value.compare("Option 2") == 0)
		{
			set appropriate variable;
			std::cerr << "Attribute Name v1 = Option 1" << std::endl;
		}
		else
			success = false;
	}
	else if(key.compare("Attribute Name v2") == 0 && successDouble)
	{
		success = true;

		myVariable = tempDouble;

		set "Attribute Name v2" to myVariable
	}

	return success;
}

void MyDevice::defineChannels()
{
}

bool MyDevice::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	return false;
}

bool MyDevice::writeChannel(unsigned short channel, const MixedValue& valuet)
{
	return false;
}

void MyDevice::definePartnerDevices()
{
}

std::string MyDevice::execute(int argc, char **argv)
{
	return parseCommandLineArgs(argc, argv);
}

void MyDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	
}

void MyDevice::stopEventPlayback()
{
}