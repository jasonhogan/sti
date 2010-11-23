/*! \file hp83711bDevice.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class hp83711bDevice
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

#include "hp83711bDevice.h"

hp83711bDevice::hp83711bDevice(ORBManager*    orb_manager, 
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

	frequency = 0;
	power = 0;

}
void hp83711bDevice::defineGpibAttributes()
{
	//addGpibAttribute("GPIB ID", "*IDN", "", true);
	//addGpibAttribute(":SYST:VERS?", ":SYST:VERS", "", true);
	addGpibAttribute("Output Power (dBm)", "POW:LEV", "", false);
	addGpibAttribute("Output Frequency (Hz)", "FREQ:CW", "", false);
}

void hp83711bDevice::defineChannels()
{
	addInputChannel(0, DataDouble);
	addInputChannel(1, DataDouble);
	addOutputChannel(2, ValueNumber);
	addOutputChannel(3, ValueNumber);
}
bool hp83711bDevice::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	//
	
	bool measureSuccess;
	std::string measurementResult;

	if(channel == 0)
	{
		measurementResult = queryDevice("FREQ:CW?");
		std::cerr << measurementResult << std::endl;
		//measurementResult.erase(0,2);
		measureSuccess = stringToValue(measurementResult, frequency, std::ios::dec, 10);
		//wavelength = wavelength * 1000000000; // multiply by 10^9
		std::cerr.precision(10);
		std::cerr << "The output frequency is:" << frequency << " Hz" << std::endl;
		dataOut.setValue(frequency);
		return measureSuccess;
	}
	else if(channel == 1)
	{
		measurementResult = queryDevice("POW:LEV?");
		std::cerr << measurementResult << std::endl;
		//measurementResult.erase(0,2);
		measureSuccess = stringToValue(measurementResult, power);
		std::cerr << "The output power is: " << power << "dBm" << std::endl;
		dataOut.setValue(power);
		return measureSuccess;
	}

	std::cerr << "Expecting either Channel 0 or 1" << std::endl;
	return false;
}
bool hp83711bDevice::writeChannel(unsigned short channel, const MixedValue& value)
{
	if(channel == 2)
	{
		double measuredValue = updateGPIBAttribute("FREQ:CW", value.getDouble(), true);
		if (measuredValue != -1)
			return true;
		else
			return false;
	}
	else if(channel == 3)
	{
		double measuredValue = updateGPIBAttribute("POW:LEV", value.getDouble(), true);
		if (measuredValue != -1)
			return true;
		else
			return false;
	}

	std::cerr << "Expecting either Channel 0 or 1" << std::endl;
	return false;
}
std::string hp83711bDevice::execute(int argc, char** argv)
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
