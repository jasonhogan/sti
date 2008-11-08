/*! \file ADF4360_Device.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class Analog_Devices_VCO::ADF4360_Device
 *  \section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
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

#include "ADF4360_Device.h"


Analog_Devices_VCO::ADF4360_Device::ADF4360_Device(
		ORBManager*		orb_manager, 
		std::string		DeviceName, 
		std::string		IPAddress,
		unsigned short	ModuleNumber,
		unsigned int VCO_Address,
		unsigned int EtraxMemoryAddress,
		unsigned short ADF4360_model) :
ADF4360(VCO_Address, EtraxMemoryAddress, ADF4360_model),
STI_Device(orb_manager, DeviceName, IPAddress, ModuleNumber)
{
}

Analog_Devices_VCO::ADF4360_Device::~ADF4360_Device()
{
}

bool Analog_Devices_VCO::ADF4360_Device::deviceMain(int argc, char **argv)
{
	return false;
}
	
void Analog_Devices_VCO::ADF4360_Device::defineAttributes()
{
	addAttribute("Fvco", getFvco());
	addAttribute("Power", "-6 dBm", "-6 dBm, -8 dBm, -11 dBm, -13 dBm, Off");
}

void Analog_Devices_VCO::ADF4360_Device::refreshAttributes()
{
	setAttribute("Fvco", getFvco());
	setAttribute("Power", getPowerStatus());

	sendLatches();
}

bool Analog_Devices_VCO::
ADF4360_Device::updateAttribute(std::string key, std::string value)
{
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);

	bool success = false;

	if(key.compare("Fvco") == 0 && successDouble)		success = setFvco(tempDouble);
	else if(key.compare("Power") == 0)
	{
		success = true;

		if(value.compare("Off") == 0)
			SynchronousPowerDown();
		else if(value.compare("-13 dBm") == 0)
		{
			success &= setOutputPower(0);
			PowerUp();
		}
		else if(value.compare("-11 dBm") == 0)
		{
			success &= setOutputPower(1);
			PowerUp();
		}
		else if(value.compare("-8 dBm") == 0)
		{
			success &= setOutputPower(2);
			PowerUp();
		}
		else if(value.compare("-6 dBm") == 0)
		{
			success &= setOutputPower(3);
			PowerUp();
		}
		else
			success = false;
	}

	return success;
}

void Analog_Devices_VCO::ADF4360_Device::defineChannels()
{
}

bool Analog_Devices_VCO::ADF4360_Device::
readChannel(STI_Server_Device::TMeasurement & Measurement)
{
	return false;
}

bool Analog_Devices_VCO::ADF4360_Device::
writeChannel(unsigned short Channel, STI_Server_Device::TDeviceEvent & Event)
{
	return false;
}

std::string Analog_Devices_VCO::ADF4360_Device::execute(int argc, char **argv)
{
	return "";
}

