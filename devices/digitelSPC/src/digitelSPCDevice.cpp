/*! \file digitelSPCDevice.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class digitelSPCDevice
 *  \section license License
 *
 *  Copyright (C) 2009 David Johnson <david.m.johnson@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  This uses code from Measurement Computing to control the USB 1408FS DAQ.
 *  The copywrite status of that code is unknown. 
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



#include "digitelSPCDevice.h"

digitelSPCDevice::digitelSPCDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber,
							unsigned short comPort) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{
	myRS232Controller = new rs232Controller(comPort);
	pressure = 1;
	voltage = 0;
	current = 0;
	
}
void digitelSPCDevice::defineAttributes() 
{
	addAttribute("Pressure", pressure); // 10L/s ion pump pressure
	addAttribute("Voltage", voltage); //
	addAttribute("Current", current); //
}

void digitelSPCDevice::refreshAttributes() 
{
	setAttribute("Pressure", pressure); // 10L/s ion pump pressure
	setAttribute("Voltage", voltage); //
	setAttribute("Current", current); //
}

bool digitelSPCDevice::updateAttribute(string key, string value)
{
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);
	bool success = false;
	bool measureSuccess = false;
	size_t length;
	char buffer[30];

	if(key.compare("Pressure") == 0)
	{
		success = true;
		result = myRS232Controller->queryDevice("~ 01 0B 33");
		if(success)
		{
			length=result.copy(buffer,6,9);
			buffer[length]='\0';
			std::cerr << "2D 10L/s Pressure is: " << string(buffer) << " Torr" << std::endl;
			measureSuccess = stringToValue( string(buffer), pressure);
		}
	}
	else if(key.compare("Current") == 0)
	{
		success = true;
		result = myRS232Controller->queryDevice("~ 01 0A 32");
		if(success)
		{
			length=result.copy(buffer,6,9);
			buffer[length]='\0';
			std::cerr << "2D 10L/s current is: " << string(buffer) << " Amps" << std::endl;
			measureSuccess = stringToValue( string(buffer), current);
		}
	}
	else if(key.compare("Voltage") == 0)
	{
		success = true;
		result = myRS232Controller->queryDevice("~ 01 0C 34");
		if(success)
		{	
			length=result.copy(buffer,4,9);
			buffer[length]='\0';
			std::cerr << "2D 10L/s Voltage is: " << string(buffer) << " Volts" << std::endl;
			measureSuccess = stringToValue( string(buffer), voltage);
		}
	}


	return success;
}

void digitelSPCDevice::defineChannels()
{
	
	//addOutputChannel(0, ValueNumber);
}
bool digitelSPCDevice::readChannel(ParsedMeasurement& Measurement)
{
	
	return false;
}
std::string digitelSPCDevice::execute(int argc, char **argv)
{
	return "";	
}
bool digitelSPCDevice::deviceMain(int argc, char **argv)
{
	return false;
}