/*! \file andoAQ6140Device.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class andoAQ6140Device
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

#include "andoAQ6140Device.h"

andoAQ6140Device::andoAQ6140Device(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber,
							std::string logDirectory,
							std::string GCipAddress,
							unsigned short GCmoduleNumber) : 
GPIB_Device(orb_manager, DeviceName, Address, ModuleNumber, logDirectory, GCipAddress, GCmoduleNumber)
{ 
	//primaryAddress = primaryGPIBAddress; //normally 1
	//secondaryAddress = 0;
	//gpibID = "Have Not Queried"; // initializes with null result - haven't checked yet
	//initialized = false;

	wavelength = 0;
	power = 0;

}
void andoAQ6140Device::defineGpibAttributes()
{
	//addGpibAttribute("Peak Power (dBm)", ":MEAS:ARR:POW", "", true);
	//addGpibAttribute("Peak Wavelength (Hz)", ":MEAS:ARR:POW:WAV", "", true);
}
void andoAQ6140Device::defineAttributes() 
{
	addAttribute("Peak Power (dBm)", power);
	addAttribute("Peak Wavelength (m)", wavelength);

}

void andoAQ6140Device::refreshAttributes() 
{
	setAttribute("Peak Power (dBm)", power);
	setAttribute("Peak Wavelength (m)", wavelength);
}

bool andoAQ6140Device::updateAttribute(string key, string value)
{
	bool success;
	MixedData data;

	success = readChannel(0, 0, data);
	wavelength = data.getDouble();

	success = readChannel(1, 0, data);
	power = data.getDouble();

	return success;
}

void andoAQ6140Device::defineChannels()
{
	addInputChannel(0, DataDouble);
	addInputChannel(1, DataDouble);
	//addOutputChannel(1, ValueNumber);
}
bool andoAQ6140Device::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	//
	
	bool measureSuccess;
	std::string measurementResult;

	if(channel == 0)
	{
		measurementResult = queryDevice(":MEAS:ARR:POW:WAV?");
		measurementResult.erase(0,2);
		measureSuccess = stringToValue(measurementResult, wavelength);
		wavelength = wavelength * 1000000000; // multiply by 10^9
		std::cerr << "The wavelength of the marker position is:" << wavelength << "m" << std::endl;
		dataOut.setValue(wavelength);
		return measureSuccess;
	}
	else if(channel == 1)
	{
		measurementResult = queryDevice(":MEAS:ARR:POW?");
		measurementResult.erase(0,2);
		measureSuccess = stringToValue(measurementResult, power);
		std::cerr << "The power at the peak is: " << power << "dBm" << std::endl;
		dataOut.setValue(power);
		return measureSuccess;
	}

	std::cerr << "Expecting either Channel 0 or 1" << std::endl;
	return false;
}
bool andoAQ6140Device::writeChannel(unsigned short channel, const MixedValue& value)
{
	return false;
}
std::string andoAQ6140Device::execute(int argc, char** argv)
{
	//command structure:  >analogIn readChannel 1
	//returns the value as a string

	if(argc < 3)
		return "Error: Invalid argument list. Expecting 'channel'.";

	int channel;
	bool channelSuccess = stringToValue(argv[2], channel);

	if(channelSuccess && channel >=0 && channel <= 1)
	{
		MixedData data;
		bool success = readChannel(channel, 0, data);

		if(success)
		{
			cerr << "Result to transfer = " << data.getDouble() << endl;
			return valueToString( data.getDouble() );
		}
		else
			return "Error: Failed when attempting to read.";
	}

	return "Error";
}
