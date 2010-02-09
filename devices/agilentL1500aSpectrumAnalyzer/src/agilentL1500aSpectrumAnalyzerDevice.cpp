/*! \file agilentL1500aSpectrumAnalyzerDevice.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class agilentL1500aSpectrumAnalyzerDevice
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



#include "agilentL1500aSpectrumAnalyzerDevice.h"

agilentL1500aSpectrumAnalyzerDevice::agilentL1500aSpectrumAnalyzerDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber,
							unsigned short primaryGPIBAddress) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{ 
	primaryAddress = primaryGPIBAddress; //normally 1
	secondaryAddress = 0;
	gpibID = "Have Not Queried"; // initializes with null result - haven't checked yet
	markerValue = 0; //assume the marker is at 0 to begin with
}

agilentL1500aSpectrumAnalyzerDevice::~agilentL1500aSpectrumAnalyzerDevice()
{
}


void agilentL1500aSpectrumAnalyzerDevice::defineAttributes() 
{
	addAttribute("GPIB ID", gpibID); //response to the ID? query
	addAttribute("Marker Value (MHz)", markerValue);
}

void agilentL1500aSpectrumAnalyzerDevice::refreshAttributes() 
{
	setAttribute("GPIB ID", gpibID); //will send the IDN? query
	setAttribute("Marker Value (MHz)", markerValue);
}

bool agilentL1500aSpectrumAnalyzerDevice::updateAttribute(string key, string value)
{
	//converts desired command into GPIB command string and executes via gpib controller partner device
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);
	bool commandSuccess;
	bool success = false;
	string result;

	if(key.compare("GPIB ID") == 0)
	{
		commandSuccess = commandDevice("FA 0 MZ; FB 500 MZ; RB 3 MZ; RL -40 dBm");
		gpibID = queryDevice("ID?");
		if(gpibID.compare("") == 0)
			success = false;
		else
			success = true;
		std::cerr << "Identification: " << gpibID << std::endl;
	}
	else if(key.compare("Marker Value (MHz)") == 0)
	{
		commandSuccess = commandDevice("MKPK HI");
		result = queryDevice("MKA?");
		if(result.compare("") == 0)
			success =  false;
		else
		{	
			commandSuccess = stringToValue(result, markerValue);
			success = true;
		}

		std::cerr << "Query result is: " << result << std::endl;
	}


	return success;
}

void agilentL1500aSpectrumAnalyzerDevice::defineChannels()
{
}

bool agilentL1500aSpectrumAnalyzerDevice::writeChannel(const RawEvent& Event)
{
	return false;
}

bool agilentL1500aSpectrumAnalyzerDevice::readChannel(DataMeasurement& Measurement)
{
	return false;
}

void agilentL1500aSpectrumAnalyzerDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	
}
void agilentL1500aSpectrumAnalyzerDevice::definePartnerDevices()
{
	addPartnerDevice("gpibController", "epLittleTable.stanford.edu", 0, "gpib"); //local name (shorthand), IP address, module #, device name as defined in main function
}

void agilentL1500aSpectrumAnalyzerDevice::stopEventPlayback()
{
}

std::string agilentL1500aSpectrumAnalyzerDevice::execute(int argc, char **argv)
{
	string commandString;
	string commandValue;
	
	int query = 0; //true (1) or false (0) if the command is expecting a response
	double measuredValue = 0;
	string result;

	//command comes as "attribute value query?"
	if(argc == 4)
	{
		commandDevice("MKPK HI");
		result = queryDevice("MKA?");
		return result;
	}
	else
		return "0"; //command needs to contain 2 pieces of information

}
bool agilentL1500aSpectrumAnalyzerDevice::deviceMain(int argc, char **argv)
{
	return false;
}
std::string agilentL1500aSpectrumAnalyzerDevice::queryDevice(std::string query)
{
	std::string queryString;
	std::string result;
	queryString = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + query + " 1";
	std::cerr << "query_str: " << queryString << std::endl;

	result = partnerDevice("gpibController").execute(queryString.c_str()); //usage: partnerDevice("lock").execute("--e1");

	return result;
}
bool agilentL1500aSpectrumAnalyzerDevice::commandDevice(std::string command)
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