/*! \file agilent8648aDevice.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class agilent8648aDevice
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



#include "agilent8648aDevice.h"

agilent8648aDevice::agilent8648aDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber, 
							unsigned short primaryGPIBAddress) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{ 
	primaryAddress = primaryGPIBAddress; //normally 19
	secondaryAddress = 0;
	outputOn = false; // default to power off
	frequency = 441.0; // in MHz
	outputPower = -7.0; // in dBm
	gpibID = "Have Not Queried"; // initializes with null result - haven't checked yet
}

agilent8648aDevice::~agilent8648aDevice()
{
}


void agilent8648aDevice::defineAttributes() 
{
	addAttribute("GPIB ID", gpibID); //response to the IDN? query
	addAttribute("Frequency (MHz)", frequency);
	addAttribute("Output", "Off", "Off, On");
	addAttribute("Output Power (dBm)", outputPower);
}

void agilent8648aDevice::refreshAttributes() 
{
	setAttribute("GPIB ID", gpibID); //response to the IDN? query
	setAttribute("Frequency (MHz)", frequency);
	setAttribute("Output", (outputOn ? "On" : "Off"));
	setAttribute("Output Power (dBm)", outputPower);
}

bool agilent8648aDevice::updateAttribute(string key, string value)
{
	//converts desired command into GPIB command string and executes via gpib controller partner device
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);
	bool commandSuccess;
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
	else if(key.compare("Frequency (MHz)") == 0)
	{
		bool successFrequency = stringToValue(value, newFrequency);
		if(successFrequency && newFrequency <= 20 && newFrequency >= 1) 
		{
			
			std::string frequencyCommand = "FREQ:CW " + value + " MHZ";
			std::cerr << "frequency_command_str: " << frequencyCommand << std::endl;
			commandSuccess = commandDevice(frequencyCommand);
			std::cerr << "device successfully commanded"<< std::endl;
			if(commandSuccess)
			{
				result = queryDevice("FREQ:CW?");
				if(result.compare("") == 0)
					success =  false;
				else
				{	
					successFrequency = stringToValue(result, frequency);
					success = true;
				}
			}
			else
				success = false;
			}
		else
		{
			std::cerr << "The desired frequency is outside of the allowed range." << std::endl;
			success = false;
		}
	}
	else if(key.compare("Output Power (dBm)") == 0)
	{
		bool successOutputPower = stringToValue(value, newOutputPower);
		if(successOutputPower && newOutputPower <= 16 && newOutputPower >= -10) 
		{
			
			std::string outputPowerCommand = ":POW:AMPL " + value + " DBM";
			std::cerr << "outputPowerCommand: " << outputPowerCommand << std::endl;
			commandSuccess = commandDevice(outputPowerCommand);
			std::cerr << "device successfully commanded"<< std::endl;
			if(commandSuccess)
			{
				result = queryDevice("POW:AMPL?");
				if(result.compare("") == 0)
					success =  false;
				else
				{	
					successOutputPower = stringToValue(result, outputPower);
					success = true;
				}
			}
			else
				success = false;
			}
		else
		{
			std::cerr << "The desired output power is outside of the allowed range." << std::endl;
			success = false;
		}
	}
	else if(key.compare("Output") == 0)
	{
		if(value.compare("On") == 0)
		{
			commandSuccess = commandDevice("OUTP:STAT ON");
			outputOn = true;
		}
		else
		{
			commandSuccess = commandDevice("OUTP:STAT OFF");
			outputOn = false;
		}
		if(commandSuccess)
		{
			success = true;
			result = queryDevice("OUTP:STAT?");
			std::cerr << "Device Status is: " << result << std::endl;
		}
	}


	return success;
}

void agilent8648aDevice::defineChannels()
{
}

bool agilent8648aDevice::writeChannel(const RawEvent& Event)
{
	return false;
}

bool agilent8648aDevice::readChannel(DataMeasurement& Measurement)
{
	return false;
}

void agilent8648aDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	
}
void agilent8648aDevice::definePartnerDevices()
{
	addPartnerDevice("gpibController", "li-gpib.stanford.edu", 12, "gpib"); //local name (shorthand), IP address, module #, device name as defined in main function
}

void agilent8648aDevice::stopEventPlayback()
{
}

std::string agilent8648aDevice::execute(int argc, char **argv)
{
	return "";
}
bool agilent8648aDevice::deviceMain(int argc, char **argv)
{
	return false;
}
std::string agilent8648aDevice::queryDevice(std::string query)
{
	std::string queryString;
	std::string result;
	queryString = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + query + " 1";
	std::cerr << "query_str: " << queryString << std::endl;

	result = partnerDevice("gpibController").execute(queryString.c_str()); //usage: partnerDevice("lock").execute("--e1");

	return result;
}
bool agilent8648aDevice::commandDevice(std::string command)
{
	std::string commandString;
	std::string result;
	commandString = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + command + " 0";

	result = partnerDevice("gpibController").execute(commandString.c_str()); //usage: partnerDevice("lock").execute("--e1");

	if(result.compare("1")==0)
		return true;
	else
		return false;
}