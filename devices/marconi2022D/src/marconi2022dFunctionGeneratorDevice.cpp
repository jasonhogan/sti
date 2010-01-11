/*! \file marconi2022dFunctionGeneratorDevice.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class marconi2022dFunctionGeneratorDevice
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



#include "marconi2022dFunctionGeneratorDevice.h"

marconi2022dFunctionGeneratorDevice::marconi2022dFunctionGeneratorDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber,
							unsigned short primaryGPIBAddress) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{ 
	primaryAddress = primaryGPIBAddress; //normally 1
	secondaryAddress = 0;
	outputOn = false; // default to power off
	frequency = 250; // in MHz
	frequencyIncrement = 1; // in MHz
	outputPower = 0.0; // in dBm
	gpibID = "Have Not Queried"; // initializes with null result - haven't checked yet
}
void marconi2022dFunctionGeneratorDevice::defineAttributes() 
{
	addAttribute("GPIB ID", gpibID); //response to the IDN? query
	addAttribute("Frequency (MHz)", frequency);
	addAttribute("Frequency Increment (MHz)", frequencyIncrement);
	addAttribute("Output", "Off", "Off, On");
	addAttribute("Output Power (dBm)", outputPower);
}

void marconi2022dFunctionGeneratorDevice::refreshAttributes() 
{
	setAttribute("GPIB ID", gpibID); //response to the IDN? query
	setAttribute("Frequency (MHz)", frequency);
	setAttribute("Frequency Increment (MHz)", frequencyIncrement);
	setAttribute("Output", (outputOn ? "On" : "Off"));
	setAttribute("Output Power (dBm)", outputPower);
}

bool marconi2022dFunctionGeneratorDevice::updateAttribute(string key, string value)
{
	//converts desired command into GPIB command string and executes via gpib controller partner device
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);
	bool commandSuccess;
	bool success = false;
	string result;

	if(key.compare("GPIB ID") == 0)
	{
		gpibID = queryDevice("SF 5");
		if(gpibID.compare("") == 0)
			success = false;
		else
			success = true;
		std::cerr << "Identification: " << gpibID << std::endl;
	}
	else if(key.compare("Frequency (MHz)") == 0)
	{
		bool successFrequency = stringToValue(value, newFrequency);
		if(successFrequency && newFrequency <= 1000 && newFrequency >= 1) 
		{
			std::string frequencyCommand = "CF " + value + " MZ";
			std::cerr << "frequency_command_str: " << frequencyCommand << std::endl;
			commandSuccess = commandDevice(frequencyCommand);
			if(commandSuccess)
			{
				result = queryDevice("QU CF");
				if(result.compare("") == 0)
					success =  false;
				else
				{	
					successFrequency = stringToValue(result, frequency);
					std::cerr << "frequency is: " << result << std::endl;
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
	else if(key.compare("Frequency Increment (MHz)") == 0)
	{
		bool successFrequencyIncrement = stringToValue(value, newFrequencyIncrement);
		if(successFrequencyIncrement && newFrequencyIncrement <= 20 && newFrequencyIncrement > 0) 
		{
			
			std::string frequencyIncrementCommand = "DE CF " + value + " MZ";
			std::cerr << "frequencyIncrementCommand_str: " << frequencyIncrementCommand << std::endl;
			commandSuccess = commandDevice(frequencyIncrementCommand);
			if(commandSuccess)
			{
				result = queryDevice("QU DE CF");
				if(result.compare("") == 0)
					success =  false;
				else
				{	
					successFrequencyIncrement = stringToValue(result, frequencyIncrement);
					std::cerr << "frequency increment is: " << result << std::endl;
					success = true;
				}
			}
			else
				success = false;
			}
		else
		{
			std::cerr << "The desired frequency increment is outside of the allowed range." << std::endl;
			success = false;
		}
	}
	else if(key.compare("Output Power (dBm)") == 0)
	{
		bool successOutputPower = stringToValue(value, newOutputPower);
		if(successOutputPower && newOutputPower <= 6 && newOutputPower >= -127) 
		{
			
			std::string outputPowerCommand = "LV " + value + " DB";
			std::cerr << "outputPowerCommand: " << outputPowerCommand << std::endl;
			commandSuccess = commandDevice(outputPowerCommand);
			std::cerr << "device successfully commanded"<< std::endl;
			if(commandSuccess)
			{
				result = queryDevice("QU LV?");
				if(result.compare("") == 0)
					success =  false;
				else
				{	
					successOutputPower = stringToValue(result, outputPower);
					std::cerr << "output power is: " << result << " dBm" << std::endl;
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
			commandSuccess = commandDevice("C1");
			outputOn = true;
		}
		else
		{
			commandSuccess = commandDevice("C0");
			outputOn = false;
		}
		
		return commandSuccess;
	}


	return success;
}
void marconi2022dFunctionGeneratorDevice::definePartnerDevices()
{
	addPartnerDevice("gpibController", "eplittletable.stanford.edu", 0, "gpib"); //local name (shorthand), IP address, module #, device name as defined in main function
}
std::string marconi2022dFunctionGeneratorDevice::queryDevice(std::string query)
{
	std::string queryString;
	std::string result;
	queryString = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + query + " 1";
	std::cerr << "query_str: " << queryString << std::endl;

	result = partnerDevice("gpibController").execute(queryString.c_str()); //usage: partnerDevice("lock").execute("--e1");

	return result;
}
bool marconi2022dFunctionGeneratorDevice::commandDevice(std::string command)
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