/*! \file GPIB_Device.cpp
 *  \author David Johnson
 *  \brief Source-file for the class GPIB_Device
 *  \section license License
 *
 *  Copyright (C) 2010 David Johnson <david.m.johnson@stanford.edu>\n
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

#include "GPIB_Device.h"

#include <sstream>
#include <string>

using std::stringstream;
using std::string;

GPIB_Device::GPIB_Device(ORBManager* orb_manager, std::string DeviceName, std::string IPAddress, 
						 unsigned short ModuleNumber, std::string logDirectory, std::string GCipAddress, 
						 unsigned short GCmoduleNumber) :
STI_Device(orb_manager, DeviceName, IPAddress, ModuleNumber, logDirectory)
{
	gpibControllerIPAddress = GCipAddress;
	gpibControllerModule = GCmoduleNumber;
	primaryAddress = ModuleNumber;
	secondaryAddress = 0;
	
	addAttributeUpdater( new GpibAttributeUpdater(this) );
}
void GPIB_Device::definePartnerDevices()
{
	addPartnerDevice("gpibController", gpibControllerIPAddress, gpibControllerModule, "gpib"); //local name (shorthand), IP address, module #, device name as defined in main function
}
std::string GPIB_Device::queryDevice(std::string query)
{
	std::string queryString;
	std::string result;
	queryString = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + query + " 1";
	std::cerr << "query_str: " << queryString << std::endl;

	result = partnerDevice("gpibController").execute(queryString.c_str()); //usage: partnerDevice("lock").execute("--e1");

	return result;
}
bool GPIB_Device::commandDevice(std::string command)
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
double GPIB_Device::updateGPIBAttribute(std::string gpibCommand, double gpibDouble, bool initialized)
{
	std::string commandString;
	std::string queryString = gpibCommand + "?";
	std::string result;
	bool successDouble;
	bool commandSuccess;
	double returnedValue;
	double tempDouble = gpibDouble;

	if(!initialized)
		{
			result = queryDevice(queryString);
			if(result.compare("") == 0)
				return -1;
			else
			{	
				successDouble = stringToValue(result, returnedValue);
				initialized = successDouble;
				tempDouble = returnedValue;
			}
		}

		commandString = gpibCommand + " " + valueToString(tempDouble);
		commandSuccess = commandDevice(commandString);
		if(commandSuccess)
		{
			result = queryDevice(queryString);
			std::cerr << queryString + " " << result << std::endl;
			successDouble = stringToValue(result, tempDouble);
			return tempDouble;
		}
		else
			return -1;
		
}
void GPIB_Device::GpibAttributeUpdater::defineAttributes()
{
	bool successDouble;

	gpibDevice->defineGpibAttributes();
	gpibAttributeMap::iterator it;
	for(it = gpibDevice->gpibAttributes.begin(); it != gpibDevice->gpibAttributes.end(); it++)
	{
		it->second.stringValue = gpibDevice->queryDevice(it->second.gpibCommand + "?");
		successDouble = stringToValue(it->second.stringValue, it->second.value); 
		if(successDouble)
			addAttribute(it->first, it->second.value, it->second.allowedValues);
		else
			addAttribute(it->first, it->second.stringValue, it->second.allowedValues);
	}
}
bool GPIB_Device::GpibAttributeUpdater::updateAttributes(std::string key, std::string value)
{
	bool successDouble;
	
	gpibAttributeMap::iterator it = gpibDevice->gpibAttributes.find(key);
	std::string commandString = it->second.gpibCommand + " " + value;
	if( gpibDevice->commandDevice(commandString) )
	{
		it->second.stringValue = gpibDevice->queryDevice(it->second.gpibCommand + "?");
		successDouble = stringToValue(it->second.stringValue, it->second.value);
		if(it->second.stringValue != "")
			return true;
		else
			return false;
	}
	else
		return false;
}
void GPIB_Device::GpibAttributeUpdater::refreshAttributes()
{
	bool successDouble;

	gpibAttributeMap::iterator it;
	for(it = gpibDevice->gpibAttributes.begin(); it != gpibDevice->gpibAttributes.end(); it++)
	{
		it->second.stringValue = gpibDevice->queryDevice(it->second.gpibCommand + "?");
		successDouble = stringToValue(it->second.stringValue, it->second.value);
		if(successDouble)
			setAttribute(it->first, it->second.value);
		else
			setAttribute(it->first, it->second.stringValue);
	}
}