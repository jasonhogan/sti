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
	rs232Bridge = new agilentRS232Bridge(comPort);
	
}
void digitelSPCDevice::defineAttributes() 
{
	addAttribute("2D 8L/S Pressure", eightLiterPressure); //8L/s ion pump pressure
}

void digitelSPCDevice::refreshAttributes() 
{
	setAttribute("2D 8L/S Pressure", eightLiterPressure); //8L/s ion pump pressure
}

bool digitelSPCDevice::updateAttribute(string key, string value)
{
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);
	bool success = false;

	if(key.compare("2D 8L/S Pressure") == 0)
	{
		success = rs232Bridge->queryDevice("~ 01 0B 2C", result);
		if(success)
			std::cerr << "2D 8L/s Pressure is: " << result << std::endl;

		bool measureSuccess = stringToValue(result, eightLiterPressure);
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