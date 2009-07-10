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



#include "vortex6000Device.h"

vortex6000Device::vortex6000Device(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{ 
	primaryAddress = 1;
	secondaryAddress = 0;
	powerOn = false; // default to power off
	laserCurrent = 47.9; // in mA
	piezoVoltage = 57.0; // in Volts
	piezoGainHigh = false; // default to low gain
	gpibID = "no name"; // initializes with no name - haven't checked yet
}

vortex6000Device::~vortex6000Device()
{
}


void vortex6000Device::defineAttributes() 
{
	addAttribute("GPIB ID", gpibID); //response to the IDN? query
	addAttribute("Laser Current (mA)", laserCurrent);
	addAttribute("Piezo Voltage (V)", piezoVoltage);
	addAttribute("Power", "Off", "Off, On");
	addAttribute("Piezo Gain", "Low", "Low, High");
}

void vortex6000Device::refreshAttributes() 
{
	setAttribute("GPIB ID", "check"); //will send the IDN? query
	setAttribute("Laser Current (mA)", laserCurrent);
	setAttribute("Piezo Voltage (V)", piezoVoltage);
	setAttribute("Power", powerOn);
	setAttribute("Piezo Gain", piezoGainHigh);
}

bool vortex6000Device::updateAttribute(string key, string value)
{
	//converts desired command into GPIB command string and executes via gpib controller partner device
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);
	bool success = false;
	string result;

	if(key.compare("GPIB ID") == 0)
	{
		gpibID = queryDevice("*idn?");
		if(gpibID.compare("") == 0)
			success = false;
		else
			success = true;
		std::cerr << "Identification: " << gpibID << std::endl;
	}
	else if(key.compare("Piezo Voltage (V)") == 0)
	{
		bool successPiezoVoltage = stringToValue(value, newPiezoVoltage);
		if(successPiezoVoltage && newPiezoVoltage < 117.5 && newPiezoVoltage > 0) 
		{
			std::string piezoCommand = ":SOUR:VOLT:PIEZ " + value;
			std::cerr << "piezo_command_str: " << piezoCommand << std::endl;
			bool commandSuccess = commandDevice(piezoCommand);
			if(commandSuccess)
			{
				result = queryDevice(":SOUR:VOLT:PIEZ?");
				if(result.compare("") == 0)
					success =  false;
				else
				{	
					commandSuccess = stringToValue(result, piezoVoltage);
					success = true;
				}
			}
			else
				success = false;
			}
		else
		{
			std::cerr << "The desired voltage is outside of the allowed range." << std::endl;
			success = false;
		}
	}
	else if(key.compare("Power") == 0)
	{
		//
	}
	else if(key.compare("Piezo Gain") == 0)
	{
	}
	else if(key.compare("Laser Current (mA)") == 0)
	{
	}

	return success;
}

void vortex6000Device::defineChannels()
{
}

bool vortex6000Device::writeChannel(const RawEvent& Event)
{
	return false;
}

bool vortex6000Device::readChannel(ParsedMeasurement& Measurement)
{
	return false;
}

void vortex6000Device::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	
}
void vortex6000Device::definePartnerDevices()
{
	addPartnerDevice("gpibController", "li-gpib.stanford.edu", 12, "gpib"); //local name (shorthand), IP address, module #, device name as defined in main function
}

void vortex6000Device::stopEventPlayback()
{
}

std::string vortex6000Device::execute(int argc, char **argv)
{
	return "";
}
bool vortex6000Device::deviceMain(int argc, char **argv)
{
	return false;
}
std::string vortex6000Device::queryDevice(std::string query)
{
	std::string queryString;
	std::string result;
	queryString = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + query + " 1";

	result = partnerDevice("gpibController").execute(queryString.c_str()); //usage: partnerDevice("lock").execute("--e1");

	return result;
}
bool vortex6000Device::commandDevice(std::string command)
{
	std::string commandString;
	std::string result;
	commandString = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + command + " 1";

	result = partnerDevice("gpibController").execute(commandString.c_str()); //usage: partnerDevice("lock").execute("--e1");

	if(result.compare("1")==0)
		return true;
	else
		return false;
}